import { Component, Input, OnInit } from "@angular/core";
import { MatDialog } from "@angular/material/dialog";
import { CreateRoomDialog } from "./room-component/dialogs/create-room-dialog copy/create-room.dialog";
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
        this.getRooms()
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

    getRooms() {
        this.roomsService.getRooms().subscribe((rooms: Room[]) => {
            this.rooms = rooms
        })
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
            (deletedRoom: Room) => {
                this.rooms.splice(this.rooms.findIndex(room => room.id == deletedRoom.id), 1)
            }
        )
    }

}