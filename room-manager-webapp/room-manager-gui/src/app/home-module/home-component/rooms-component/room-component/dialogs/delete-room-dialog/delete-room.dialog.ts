import { Component, ComponentFactoryResolver, Inject } from "@angular/core";
import { MatDialogRef, MAT_DIALOG_DATA } from "@angular/material/dialog";
import { defaultRoom, Room } from "../../models/room.model";

export interface DeleteRoomDialogData {
  room: Room
}

@Component({
  selector: 'delete-room',
  templateUrl: 'delete-room.dialog.html',
  styleUrls: ['delete-room.dialog.html'],
})
export class DeleteRoomDialog {

  room: Room = defaultRoom 

  constructor(
    public dialogRef: MatDialogRef<DeleteRoomDialog>,
    @Inject(MAT_DIALOG_DATA) public data: DeleteRoomDialogData
  ) {
    this.room = data.room
  }

  onConfirmClick(): void {
    this.dialogRef.close(true);
  }

  onCancelClick(): void {
    this.dialogRef.close(false);
  }
}