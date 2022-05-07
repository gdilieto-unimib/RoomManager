import { FormArray, FormControl, FormGroup } from "@angular/forms";
import { Form } from "src/app/shared-module/classes/form.class";
import { Room } from "../room-component/models/room.model";

export class RoomForm extends Form<Room>{
    createForm(room: Room): FormGroup {
        this.form.addControl('id', new FormControl(room.id))
        this.form.addControl('name', new FormControl(room.name))
        this.form.addControl('ipv4', new FormControl(room.ipv4))
        this.form.addControl('connected', new FormControl(room.connected))
        this.form.addControl('ssid', new FormControl(room.ssid))
        this.form.addControl('password', new FormControl(room.password))
        this.form.addControl('sensors', new FormControl([]))
        //this.form.addControl('sensors', this.formBuilder.array(room.sensors))

        return this.form
    }

    updateForm(room: Room): FormGroup {
        this.form.patchValue(room)

        return this.form
    }
}