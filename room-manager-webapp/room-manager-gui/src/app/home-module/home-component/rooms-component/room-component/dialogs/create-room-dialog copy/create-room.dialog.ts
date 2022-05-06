import { Component, ComponentFactoryResolver, Inject } from "@angular/core";
import { MatDialogRef, MAT_DIALOG_DATA } from "@angular/material/dialog";
import { defaultRoom, Room } from "../../models/room.model";
import { Form } from "src/app/shared-module/classes/form.class";
import { RoomForm } from "../../../form/room-form.class";
import { FormBuilder } from "@angular/forms";

export interface CreateRoomDialogData {
}

@Component({
  selector: 'create-room',
  templateUrl: 'create-room.dialog.html',
  styleUrls: ['create-room.dialog.html'],
})
export class CreateRoomDialog {

  room: Room = defaultRoom 
  form: Form<Room> = new RoomForm(this.formBuilder)

  constructor(
    public dialogRef: MatDialogRef<CreateRoomDialog>,
    @Inject(MAT_DIALOG_DATA) public data: CreateRoomDialogData,
    private formBuilder: FormBuilder
  ) {
    this.form.createForm(this.room)
  }

  onConfirmClick(): void {
    this.dialogRef.close(this.form.getFormValue());
  }

  onCancelClick(): void {
    this.dialogRef.close(null);
  }
}