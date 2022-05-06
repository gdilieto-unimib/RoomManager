import { Component, Input } from "@angular/core";
import { MatDialog } from "@angular/material/dialog";
import { roomsMock } from "./mocks/rooms.mock";
import { CreateRoomDialog } from "./room-component/dialogs/create-room-dialog copy/create-room.dialog";
import { Room } from "./room-component/models/room.model";

@Component({
    selector: 'app-rooms',
    templateUrl: './rooms.component.html',
    styleUrls: ['./rooms.component.scss']
})
export class RoomsComponent {
    @Input() rooms: Room[] = roomsMock

    constructor(public dialog: MatDialog) {}

    onEditedRoom(editedRoom: Room) {
        this.rooms[this.rooms.findIndex(room => room.id == editedRoom.id)] = editedRoom
    }

    onDeletedRoom(deletedRoom: Room) {
        this.rooms.splice(this.rooms.findIndex(room => room.id == deletedRoom.id), 1)
    }

    onCreateClick() {
        const dialogRef = this.dialog.open(CreateRoomDialog, {
            data: {
            }
        });

        dialogRef.afterClosed().subscribe(createdRoom => {
            if(createdRoom) {
                this.rooms.push(createdRoom)
            }
        });
    }

}