import { Sensor } from "../sensors-component/sensor-component/models/sensor.model";
import { Alarm } from "./alarm.model";

export interface Room{
    id?: number,
    name: string,
    ipv4: string,
    connected?: boolean,
    monitoring?: boolean,
    ssid?: string,
    password?: string,
    sensors: Sensor[]
    alarms: Alarm[]
}

export const defaultRoom : Room = {
    name: "",
    ipv4: "",
    sensors: [],
    alarms: []
}