import { Component, Input, OnInit } from "@angular/core";
import { MatDialog } from "@angular/material/dialog";
import { CreateRoomDialog } from "./room-component/dialogs/create-room-dialog/create-room.dialog";
import { Room } from "./room-component/models/room.model";
import { RoomsService } from "./services/rooms.service";

@Component({
    selector: 'app-rooms',
    templateUrl: './rooms.component.html',
    styleUrls: ['./rooms.component.scss']
})
export class RoomsComponent implements OnInit{
    rooms: Room[] = []

    constructor(
        public dialog: MatDialog,
        public roomsService: RoomsService
    ) {}

    ngOnInit() {
        this.roomsPolling();
        //this.measuresPolling();
    }

    roomsPolling() {
        if (this.roomsService.updatingControl) {
            setTimeout(
                () => {
                    this.roomsPolling();
                }, 1000
            )
        } else {
            this.getRooms();
            setTimeout(
                () => {
                    this.roomsPolling();
                }, 10000
            )
        }
    }

    measuresPolling() {
        if (this.roomsService.updatingControl) {
            setTimeout(
                () => {
                    this.measuresPolling();
                }, 1000
            )
        } else {
            this.getRoomsMeasures();
            setTimeout(
                () => {
                    this.measuresPolling();
                }, 2000
            )
        }
    }

    onEditedRoom(editedRoom: Room) {
        this.updateRoom(editedRoom)
    }

    onDeletedRoom(deletedRoom: Room) {
        this.deleteRoom(deletedRoom)
    }

    onCreateClick() {
        const dialogRef = this.dialog.open(CreateRoomDialog, {
            data: {
            }
        });

        dialogRef.afterClosed().subscribe((createdRoom: Room) => {
            if(createdRoom) {
                this.addRoom(createdRoom)
            }
        });
    }

    addRoom(createdRoom: Room) {
        this.roomsService.addRoom(createdRoom).subscribe(
            (createdRoom: Room) => {
                this.rooms.push(createdRoom)
            }
        )
    }

    updateRoom(updatedRoom: Room) {
        this.roomsService.updateRoom(updatedRoom).subscribe(
            (updatedRoom: Room) => {
                this.rooms[this.rooms.findIndex(room => room.id == updatedRoom.id)] = updatedRoom
            }
        )
    }
    
    deleteRoom(deletedRoom: Room) {
        this.roomsService.deleteRoom(deletedRoom).subscribe(
            (idDeletedRoom: number) => {
                this.rooms.splice(this.rooms.findIndex(room => room.id == idDeletedRoom), 1)
            }
        )
    }

    getRooms() {
        this.roomsService.getRooms().subscribe(
            (rooms) => {
                if(!this.roomsService.updatingControl) {
                    this.rooms = rooms.map(
                        room => {
                            room.connected = !!room.connected
                            room.monitoring = !!room.monitoring
                            return room
                        }
                    )
                }
            }
        );
    }
    
    getRoomsMeasures() {
        this.roomsService.getRooms().subscribe(
            (rooms) => {
                if(!this.roomsService.updatingControl) {
                    for(let room of rooms) {
                        for(let sensor of room.sensors) {
                            let matchedSensor = this.rooms.find(room2 => room2.id==room.id)?.sensors.find(sensor2 => sensor2.id == sensor.id)
                            if (matchedSensor) {
                                matchedSensor.measure = sensor.measure
                                console.log(matchedSensor.measure)
                            } 
                        }
                    }
                }
            }
        );
    }

}