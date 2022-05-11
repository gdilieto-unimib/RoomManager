import { Component, Input } from "@angular/core";
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
    @Input() roomIp: string = "";

    constructor(
        public roomsService: RoomsService
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

    setAuto(active: boolean) {
        if (active) {
            if(this.sensor.type == SensorType.Light) {
                this.roomsService.setRoomLightAuto(this.roomIp).subscribe(
                    (response) => {
                    }
                )
            }
        } else {
            this.sensor.active = false
            this.setOnOff(this.sensor.active);
        }
        
            
    }

    setOnOff(active: boolean) {
        if (active) {
            if(this.sensor.type == SensorType.Light) {
                this.roomsService.setRoomLightOn(this.roomIp).subscribe(
                    (response) => {
                    }
                )
            }
            
            if(this.sensor.type == SensorType.Temperature) {
                this.roomsService.setRoomTempOn(this.roomIp).subscribe(
                    (response) => {
                    }
                )
            }
        } else {
            if(this.sensor.type == SensorType.Light) {
                this.roomsService.setRoomLightOff(this.roomIp).subscribe(
                    (response) => {
                    }
                )
            }
            console.log(this.sensor.type)
            if(this.sensor.type == SensorType.Temperature) {
                this.roomsService.setRoomTempOff(this.roomIp).subscribe(
                    (response) => {
                    }
                )
            }
        }
            
    }

}