import { Component, Inject } from "@angular/core";
import { MatDialogRef, MAT_DIALOG_DATA } from "@angular/material/dialog";
import { defaultRoom, Room } from "../../models/room.model";
import { Form } from "src/app/shared-module/classes/form.class";
import { RoomForm } from "../../../form/room-form.class";
import { FormBuilder } from "@angular/forms";
import { Sensor, SensorType } from "../../sensors-component/sensor-component/models/sensor.model";

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
  hide: boolean = true
  typesOfSensors : SensorType[] = Object.values(SensorType);

  constructor(
    public dialogRef: MatDialogRef<CreateRoomDialog>,
    @Inject(MAT_DIALOG_DATA) public data: CreateRoomDialogData,
    private formBuilder: FormBuilder
  ) {
    this.form.createForm(this.room)
  }

  getSensor(sensorType: SensorType): Sensor {
    return {
      type: sensorType,
      name: sensorType.toString()
    }
  }

  onConfirmClick(): void {
    console.log(this.form.getFormValue())
    this.dialogRef.close(this.form.getFormValue());
  }

  onCancelClick(): void {
    this.dialogRef.close(null);
  }
}