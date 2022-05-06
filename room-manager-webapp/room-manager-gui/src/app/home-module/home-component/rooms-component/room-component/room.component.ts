import { Component, EventEmitter, Input, Output } from "@angular/core";
import { MatDialog } from "@angular/material/dialog";
import { DeleteRoomDialog } from "./dialogs/delete-room-dialog/delete-room.dialog";
import { EditRoomDialog } from "./dialogs/edit-room-dialog/edit-room.dialog";
import { defaultRoom, Room } from "./models/room.model";

@Component({
    selector: 'app-room',
    templateUrl: './room.component.html',
    styleUrls: ['./room.component.scss']
})
export class RoomComponent{
    @Input() room: Room = defaultRoom
    @Output() editedRoom: EventEmitter<Room> = new EventEmitter()
    @Output() deletedRoom: EventEmitter<Room> = new EventEmitter()
    
    constructor(public dialog: MatDialog) {}

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
}