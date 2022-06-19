import { Component, Input } from "@angular/core";
import { Actuator, defaultActuator } from "./sensor-component/models/actuator.model";
import { Sensor } from "./sensor-component/models/sensor.model";

@Component({
    selector: 'app-sensors',
    templateUrl: './sensors.component.html',
    styleUrls: ['./sensors.component.css']
})
export class SensorsComponent{
    @Input() sensors: Sensor[] = []
    @Input() actuators: Actuator[] = []
    @Input() roomConnected: boolean = false;
    @Input() roomId: number = -1;

    constructor() {
    }

    getActuator(sensor: Sensor): Actuator {
        let actuator = this.actuators.find(actuator => actuator.type == sensor.type);
        return actuator?actuator:defaultActuator;
    }
}