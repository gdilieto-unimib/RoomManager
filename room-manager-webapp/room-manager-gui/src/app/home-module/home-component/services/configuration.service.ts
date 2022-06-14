import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Injectable, OnInit } from '@angular/core';
import { Observable, tap } from 'rxjs';
import { environment } from 'src/environments/environment';
import { Configuration, defaultConfiguration } from '../rooms-component/models/configuration.model';

@Injectable({
  providedIn: 'root',
})
export class ConfigurationService {

  _configuration : Configuration = defaultConfiguration

  constructor(private http: HttpClient) { 
    this.initConfiguration();
  }

  configurationUrl = environment.apiUrl+'/configuration'

  initConfiguration() {
    return this.http.get<Configuration>(this.configurationUrl).subscribe(
      configuration => this.configuration = configuration
    )
  }

  setSingleMode(singleMode : boolean): Observable<any> {
    return this.http.post<any>(this.configurationUrl+"/singleMode", {singleMode: singleMode}).pipe(
      tap(singleMode => {
        this.configuration.singleMode = singleMode
      })
    );
  }

  setEcoMode(ecoMode : boolean): Observable<any> {
    return this.http.post<any>(this.configurationUrl+"/ecoMode", {ecoMode: ecoMode}).pipe(
      tap(singleMode => {
        this.configuration.singleMode = singleMode
      })
    );
  }
  
  setSleepMode(sleepMode : boolean): Observable<any> {
    return this.http.post<any>(this.configurationUrl+"/sleepMode", {sleepMode: sleepMode}).pipe(
      tap(sleepMode => {
        this.configuration.sleepMode = sleepMode
      })
    );
  }

  setSleepSchedule(sleepSchedule: String): Observable<any> {
    return this.http.post<any>(this.configurationUrl+"/schedule", {sleepSchedule: sleepSchedule}).pipe(
      tap(schedule => {
        this.configuration.sleepSchedule = sleepSchedule
      })
    );
  }

  get configuration(): Configuration {
    return this._configuration
  }

  set configuration(configuration) {
    this._configuration = configuration
  }
}