import { Pipe, PipeTransform } from '@angular/core';
import { SensorType } from '../models/sensor.model';

@Pipe({name: 'sensorIcon'})
export class SensorIconPipe implements PipeTransform {
  typeIconMappings = [[SensorType.Temperature, 'device_thermostat'], [SensorType.Light, 'lightbulb'], [SensorType.Wifi, 'wifi']]

  transform(sensorType: SensorType | null | undefined): string {
    let typeIconMapping = this.typeIconMappings.find(mapping => mapping[0]==sensorType)
    return typeIconMapping ? typeIconMapping[1] : ''
  }
}