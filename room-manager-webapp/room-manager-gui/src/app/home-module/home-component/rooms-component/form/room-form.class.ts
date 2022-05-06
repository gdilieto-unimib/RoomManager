import { FormControl, FormGroup, Validators } from "@angular/forms";
import { Form } from "src/app/shared-module/classes/form.class";
import { Room } from "../room-component/models/room.model";

export class RoomForm extends Form<Room>{
    createForm(room: Room): FormGroup {
        this.form.addControl('id', new FormControl(room.id))
        this.form.addControl('name', new FormControl(room.name))
        this.form.addControl('ip', new FormControl(room.ip))
        this.form.addControl('sensors', this.formBuilder.array(room.sensors))

        return this.form
    }

    updateForm(room: Room): FormGroup {
        this.form.patchValue(room)

        return this.form
    }
}