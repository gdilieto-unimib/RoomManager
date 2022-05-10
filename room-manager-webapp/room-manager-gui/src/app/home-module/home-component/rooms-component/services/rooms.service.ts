import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { environment } from 'src/environments/environment';
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
  
  addRoom(room: Room): Observable<Room> {
    return this.http.put<Room>(this.roomUrl, room);
  }

  updateRoom(room: Room): Observable<Room> {
    return this.http.post<Room>(this.roomUrl, room);
  }

  deleteRoom(room: Room): Observable<number> {
    return this.http.delete<number>(this.roomUrl+'/'+room.id);
  }

  getIsRoomConnected(roomIp: string): Observable<boolean> {
    return this.http.get<boolean>(this.roomUrl+'/'+roomIp+'/connected');
  }
}