import { Component, EventEmitter, Input, OnInit, Output } from "@angular/core";
import { MatDialog } from "@angular/material/dialog";
import { RoomsService } from "../services/rooms.service";
import { DeleteRoomDialog } from "./dialogs/delete-room-dialog/delete-room.dialog";
import { EditRoomDialog } from "./dialogs/edit-room-dialog/edit-room.dialog";
import { defaultRoom, Room } from "./models/room.model";

@Component({
    selector: 'app-room',
    templateUrl: './room.component.html',
    styleUrls: ['./room.component.scss']
})
export class RoomComponent implements OnInit {
    @Input() room: Room = defaultRoom
    @Output() editedRoom: EventEmitter<Room> = new EventEmitter()
    @Output() deletedRoom: EventEmitter<Room> = new EventEmitter()
    
    isConnecting = false;
    isUpdatePolling = false

    constructor(
        public dialog: MatDialog,
        public roomsService: RoomsService
    ) {}

    ngOnInit() {
        this.isConnectedRoomPolling()
    }

    isConnectedRoomPolling(): void {
        this.getIsRoomConnected(this.room.ipv4).subscribe(
            (isConnected) => {
                this.room.connected = isConnected
                if (this.room.connected) {
                    if (this.isUpdatePolling == false) {
                        this.updateRoomPolling()
                    } 
                    this.isUpdatePolling = true
                } else {
                    this.isUpdatePolling = false
                }
            }
        )
        setTimeout(() => {
            this.isConnectedRoomPolling();
        },5000);
    }

    updateRoomPolling(): void {
        if(this.room.connected) {
            this.getRoomSensors(this.room.id?this.room.id:0).subscribe(
                (sensors) => {
                    this.room.sensors = sensors
                }
            )
            setTimeout(() => {
                this.updateRoomPolling();
            },5000);
        }
    }

    toggleWifi(): void {
        this.isConnecting = true
        if (this.room.connected) {
            setTimeout(()=>{ this.room.connected = false, this.isConnecting = false }, 4000) 
        } else {
            setTimeout(()=>{ this.room.connected = true, this.isConnecting = false }, 4000)   
        }
    }

    editRoom(): void {
        const dialogRef = this.dialog.open(EditRoomDialog, {
            data: {
                room: this.room
            }
        });

        dialogRef.afterClosed().subscribe(editedRoom => {
            if(editedRoom) {
                this.editedRoom.emit(editedRoom)
            }
        });
    } 
    
    deleteRoom() {
        const dialogRef = this.dialog.open(DeleteRoomDialog, {
            data: {
                room: this.room
            }
        });

        dialogRef.afterClosed().subscribe(deletedRoom => {
            if(deletedRoom) {
                this.deletedRoom.emit(this.room)
            }
        });
    }

    getRoom(roomId: number) {
        return this.roomsService.getRoom(roomId)
    }

    getRoomSensors(roomId: number) {
        return this.roomsService.getRoomSensors(roomId)
    }

    getIsRoomConnected(roomIp: string) {
        return this.roomsService.getIsRoomConnected(roomIp)
    }
}