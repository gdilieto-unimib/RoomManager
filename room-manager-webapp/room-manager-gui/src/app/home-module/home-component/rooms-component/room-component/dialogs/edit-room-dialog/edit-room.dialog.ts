import { Component, ComponentFactoryResolver, Inject } from "@angular/core";
import { MatDialogRef, MAT_DIALOG_DATA } from "@angular/material/dialog";
import { defaultRoom, Room } from "../../models/room.model";
import { Form } from "src/app/shared-module/classes/form.class";
import { RoomForm } from "../../../form/room-form.class";
import { FormBuilder } from "@angular/forms";

export interface EditRoomDialogData {
  room: Room
}

@Component({
  selector: 'edit-room',
  templateUrl: 'edit-room.dialog.html',
  styleUrls: ['edit-room.dialog.html'],
})
export class EditRoomDialog {

  room: Room = defaultRoom 
  form: Form<Room> = new RoomForm(this.formBuilder)

  constructor(
    public dialogRef: MatDialogRef<EditRoomDialog>,
    @Inject(MAT_DIALOG_DATA) public data: EditRoomDialogData,
    private formBuilder: FormBuilder
  ) {
    this.room = data.room
    this.form.createForm(this.room)
  }

  onConfirmClick(): void {
    this.dialogRef.close(this.form.getFormValue());
  }

  onCancelClick(): void {
    this.dialogRef.close(null);
  }
}