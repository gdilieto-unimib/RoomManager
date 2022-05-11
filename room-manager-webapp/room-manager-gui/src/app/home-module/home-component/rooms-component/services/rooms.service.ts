import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { environment } from 'src/environments/environment';
import { Alarm } from '../room-component/models/alarm.model';
import { Room } from '../room-component/models/room.model';
import { Sensor } from '../room-component/sensors-component/sensor-component/models/sensor.model';

@Injectable({
  providedIn: 'root',
})
export class RoomsService {

  constructor(private http: HttpClient) { }

  roomUrl = environment.apiUrl+'/rooms'

  getRooms(): Observable<Room[]> {
    return this.http.get<Room[]>(this.roomUrl);
  }

  getRoom(roomId: number): Observable<Room> {
    return this.http.get<Room>(this.roomUrl+'/'+roomId);
  }

  getRoomSensors(roomId: number): Observable<Sensor[]> {
    return this.http.get<Sensor[]>(this.roomUrl+'/'+roomId+'/sensors');
  }

  getRoomAlarms(roomId: number): Observable<Alarm[]> {
    return this.http.get<Alarm[]>(this.roomUrl+'/'+roomId+'/alarms');
  }
  
  addRoom(room: Room): Observable<Room> {
    return this.http.put<Room>(this.roomUrl, room);
  }

  updateRoom(room: Room): Observable<Room> {
    return this.http.post<Room>(this.roomUrl, room);
  }

  deleteRoom(room: Room): Observable<number> {
    return this.http.delete<number>(this.roomUrl+'/'+room.id);
  }

  getIsRoomConnected(roomIp: string): Observable<any> {
    return this.http.get<any>(`http://${roomIp}:80/IsConnected`);
  }

  setRoomTempOn(roomIp: string): Observable<any> {
    return this.http.get<any>(`http://${roomIp}:80/OnTemp`);
  }

  setRoomTempOff(roomIp: string): Observable<any> {
    return this.http.get<any>(`http://${roomIp}:80/OffTemp`);
  }

  setRoomLightOn(roomIp: string): Observable<any> {
    return this.http.get<any>(`http://${roomIp}:80/OnLight`);
  }

  setRoomLightOff(roomIp: string): Observable<any> {
    return this.http.get<any>(`http://${roomIp}:80/OffLight`);
  }
  
  setRoomLightAuto(roomIp: string): Observable<any> {
    return this.http.get<any>(`http://${roomIp}:80/AutoLight`);
  }

  postStartRoomMonitoring(roomIp: string): Observable<any> {
    return this.http.get<any>(`http://${roomIp}:80/StartMonitoring`);
  }

  postStopRoomMonitoring(roomIp: string): Observable<any> {
    return this.http.get<any>(`http://${roomIp}:80/StopMonitoring`);
  }
}