import { Component, Input } from "@angular/core";
import { defaultSensor, Sensor } from "./models/sensor.model";

@Component({
    selector: 'app-sensor',
    templateUrl: './sensor.component.html',
    styleUrls: ['./sensor.component.css']
})
export class SensorComponent{
    @Input() sensor: Sensor = defaultSensor
}