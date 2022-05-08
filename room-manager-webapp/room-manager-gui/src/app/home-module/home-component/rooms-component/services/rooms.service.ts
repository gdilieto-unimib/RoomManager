import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { environment } from 'src/environments/environment';
import { Room } from '../room-component/models/room.model';

@Injectable({
  providedIn: 'root',
})
export class RoomsService {

  constructor(private http: HttpClient) { }

  roomUrl = environment.apiUrl+'/rooms'

  getRooms(): Observable<Room[]> {
    return this.http.get<Room[]>(this.roomUrl);
  }
  
  addRoom(room: Room): Observable<Room> {
    return this.http.put<Room>(this.roomUrl, room);
  }

  updateRoom(room: Room): Observable<Room> {
    return this.http.post<Room>(this.roomUrl, room);
  }

  deleteRoom(room: Room): Observable<Room> {
    return this.http.delete<Room>(this.roomUrl+'/'+room.id);
  }
}