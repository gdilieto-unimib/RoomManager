import { Component, ComponentFactoryResolver, Inject } from "@angular/core";
import { MatDialogRef, MAT_DIALOG_DATA } from "@angular/material/dialog";
import { defaultRoom, Room } from "../../models/room.model";
import { Form } from "src/app/shared-module/classes/form.class";
import { RoomForm } from "../../../form/room-form.class";
import { FormBuilder } from "@angular/forms";
import { defaultSensor, Sensor, SensorType } from "../../sensors-component/sensor-component/models/sensor.model";
import { RoomsService } from "../../../services/rooms.service";

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
  hide: boolean = true
  isTestingConnection = true
  isConnected = true
  typesOfSensors : SensorType[] = Object.values(SensorType);

  constructor(
    public dialogRef: MatDialogRef<EditRoomDialog>,
    @Inject(MAT_DIALOG_DATA) public data: EditRoomDialogData,
    private formBuilder: FormBuilder,
    private roomsService: RoomsService
  ) {
    this.room = data.room
    this.form.createForm(this.room)
  }

  getSensor(sensorType: SensorType): Sensor {
    let sensor = this.room.sensors.find(sensor => sensor.type == sensorType)
    return sensor ? sensor : {
      ...defaultSensor,
      type: sensorType,
      name: sensorType.toString(),
    }
  }

  onConfirmClick(): void {
    console.log(this.form.getFormValue())
    this.dialogRef.close(this.form.getFormValue());
  }

  onCancelClick(): void {
    this.dialogRef.close(null);
  }

  onTestConnectionClick(): void {
    this.isTestingConnection = true
    this.getIsRoomConnected(this.form.getValue('ipv4')).subscribe(
      (isConnected: any) => {
        this.isTestingConnection = false
        this.isConnected = isConnected.connected
      }
    )
  }

  getIsRoomConnected(roomIp: string) {
    return this.roomsService.getIsRoomConnected(roomIp)
  }
}