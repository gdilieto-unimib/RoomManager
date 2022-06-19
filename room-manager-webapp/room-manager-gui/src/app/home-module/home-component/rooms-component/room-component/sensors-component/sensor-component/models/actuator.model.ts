export enum ActuatorType {
    Temperature = 'Temperature',
    Light = 'Light'
}

export interface Actuator{
    id?: number,
    type?: ActuatorType,
    name: string,
    config?: number,
    active: boolean
    auto: boolean
}

export const defaultActuator: Actuator = {
    name: "",
    active: false,
    auto: false
}