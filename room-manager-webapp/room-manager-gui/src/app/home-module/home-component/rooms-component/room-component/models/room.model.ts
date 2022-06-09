import { Sensor } from "../sensors-component/sensor-component/models/sensor.model";
import { Alarm } from "./alarm.model";

export interface Room{
    id?: number,
    name: string,
    mac: string,
    connected?: boolean,
    monitoring?: boolean,
    sensors: Sensor[]
    alarms: Alarm[]
}

export const defaultRoom : Room = {
    name: "",
    mac: "",
    sensors: [],
    alarms: []
}