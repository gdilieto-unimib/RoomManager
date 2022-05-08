import { Measure } from "./measure.model"

export enum SensorType {
    Temperature = 'Temperature',
    Light = 'Light',
    Wifi = 'Wifi'
}

export interface Sensor{
    id?: number,
    type?: SensorType,
    name: string,
    measure?: Measure,
    auto: boolean,
    active: boolean
}

export const defaultSensor: Sensor = {
    name: "",
    auto: false,
    active: false
}