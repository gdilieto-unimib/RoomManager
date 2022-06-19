import { Component, Input } from "@angular/core";
import { ErrorService } from "src/app/home-module/home-component/services/error.service";
import { RoomsService } from "../../../services/rooms.service";
import { Actuator, defaultActuator } from "./models/actuator.model";
import { defaultSensor, Sensor, SensorType } from "./models/sensor.model";

@Component({
    selector: 'app-sensor',
    templateUrl: './sensor.component.html',
    styleUrls: ['./sensor.component.css']
})
export class SensorComponent{
    @Input() sensor: Sensor = defaultSensor
    @Input() actuator: Actuator = defaultActuator
    @Input() roomConnected: boolean = false;
    @Input() roomId: number = -1;

    constructor(
        public roomsService: RoomsService,
        public errorService: ErrorService
    ) {

    }

    toggleAuto() {
        this.actuator.auto = !this.actuator.auto
        this.setAuto(this.actuator.auto)
    }

    toggleActive() {
        this.actuator.active = !this.actuator.active
        this.setOnOff(this.actuator.active)
    }

    setAuto(auto: boolean) {
        this.roomsService.updatingControl = true
        if (auto) {
            this.roomsService.setActuatorAuto(this.roomId, this.actuator.id?this.actuator.id:-1).subscribe(
                (response) => {
                    this.roomsService.updatingControl = false
                },
                (err) => {
                    this.roomsService.updatingControl = false
                    this.errorService.showErrors(err.error)
                }
            )
        } else {
            this.actuator.active=false
            this.setOnOff(false)        
        }            
    }

    setOnOff(active: boolean) {
        this.roomsService.updatingControl = true
        if (active) {
            this.roomsService.setActuatorOn(this.roomId, this.actuator.id?this.actuator.id:-1).subscribe(
                (response) => {
                    this.roomsService.updatingControl = false
                },
                (err) => {
                    this.roomsService.updatingControl = false
                    this.errorService.showErrors(err.error)
                }
            )
        } else {
            this.roomsService.setActuatorOff(this.roomId, this.actuator.id?this.actuator.id:-1).subscribe(
                (response) => {
                    this.roomsService.updatingControl = false
                },
                (err) => {
                    this.roomsService.updatingControl = false
                    this.errorService.showErrors(err.error)
                }
            )
        }
            
    }

}