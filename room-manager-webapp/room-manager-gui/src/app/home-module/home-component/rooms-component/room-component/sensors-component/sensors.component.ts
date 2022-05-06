import { Component, Input } from "@angular/core";
import { Sensor } from "./sensor-component/models/sensor.model";

@Component({
    selector: 'app-sensors',
    templateUrl: './sensors.component.html',
    styleUrls: ['./sensors.component.css']
})
export class SensorsComponent{
    @Input() sensors: Sensor[] = []

    constructor() {
    }
}