import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { environment } from 'src/environments/environment';

@Injectable({
  providedIn: 'root',
})
export class ConfigurationService {

  constructor(private http: HttpClient) { }

  configurationUrl = environment.apiUrl+'/configuration'

  setSingleMode(singleMode : boolean): Observable<any> {
    return this.http.post<any>(this.configurationUrl+"/singleMode", {singleMode: singleMode});
  }

  setEcoMode(ecoMode : boolean): Observable<any> {
    return this.http.post<any>(this.configurationUrl+"/ecoMode", {ecoMode: ecoMode});
  }
}