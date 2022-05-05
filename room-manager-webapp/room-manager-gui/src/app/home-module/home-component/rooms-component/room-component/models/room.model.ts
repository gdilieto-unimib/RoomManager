import { Sensor } from "../sensors-component/sensor-component/models/sensor.model";

export interface Room{
    name: string,
    ip: string,
    sensors: Sensor[]
}

export const defaultRoom : Room = {
    name: "",
    ip: "",
    sensors: []
}