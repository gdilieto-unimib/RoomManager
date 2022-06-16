import { Component, Input } from "@angular/core";
import { ErrorService } from "src/app/home-module/home-component/services/error.service";
import { RoomsService } from "../../../services/rooms.service";
import { defaultSensor, Sensor, SensorType } from "./models/sensor.model";

@Component({
    selector: 'app-sensor',
    templateUrl: './sensor.component.html',
    styleUrls: ['./sensor.component.css']
})
export class SensorComponent{
    @Input() sensor: Sensor = defaultSensor
    @Input() roomConnected: boolean = false;
    @Input() roomId: number = -1;

    constructor(
        public roomsService: RoomsService,
        public errorService: ErrorService
    ) {

    }

    toggleAuto() {
        this.sensor.auto = !this.sensor.auto
        this.setAuto(this.sensor.auto)
    }

    toggleActive() {
        this.sensor.active = !this.sensor.active
        this.setOnOff(this.sensor.active)
    }

    setAuto(auto: boolean) {
        this.roomsService.updatingControl = true
        if (auto) {
            this.roomsService.setSensorAuto(this.sensor.id?this.sensor.id:-1).subscribe(
                (response) => {
                    this.roomsService.updatingControl = false
                },
                (err) => {
                    this.roomsService.updatingControl = false
                    this.errorService.showErrors(err.error)
                }
            )
        } else {
            this.setOnOff(this.sensor.active);        
        }            
    }

    setOnOff(active: boolean) {
        this.roomsService.updatingControl = true
        if (active) {
            this.roomsService.setSensorOn(this.sensor.id?this.sensor.id:-1).subscribe(
                (response) => {
                    this.roomsService.updatingControl = false
                },
                (err) => {
                    this.roomsService.updatingControl = false
                    console.log(err.error)
                    this.errorService.showErrors(err.error)
                }
            )
        } else {
            this.roomsService.setSensorOff(this.sensor.id?this.sensor.id:-1).subscribe(
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