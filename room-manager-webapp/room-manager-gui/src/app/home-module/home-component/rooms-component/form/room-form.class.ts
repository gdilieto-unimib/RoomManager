import { FormArray, FormControl, FormGroup } from "@angular/forms";
import { Form } from "src/app/shared-module/classes/form.class";
import { Room } from "../room-component/models/room.model";

export class RoomForm extends Form<Room>{
    createForm(room: Room): FormGroup {
        this.form.addControl('id', new FormControl(room.id))
        this.form.addControl('name', new FormControl(room.name))
        this.form.addControl('mac', new FormControl(room.mac))
        this.form.addControl('monitoring', new FormControl(room.monitoring))
        this.form.addControl('connected', new FormControl(room.connected))
        this.form.addControl('sensors', new FormControl(room.sensors))
        this.form.addControl('actuators', new FormControl(room.actuators))
        //this.form.addControl('sensors', this.formBuilder.array(room.sensors))

        return this.form
    }

    updateForm(room: Room): FormGroup {
        this.form.patchValue(room)

        return this.form
    }
}