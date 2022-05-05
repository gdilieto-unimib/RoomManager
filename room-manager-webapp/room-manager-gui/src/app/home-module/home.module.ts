import { NgModule } from "@angular/core";
import { CommonModule } from "@angular/common";

import { MatToolbarModule } from '@angular/material/toolbar';
import { MatIconModule } from '@angular/material/icon';
import { MatSidenavModule } from '@angular/material/sidenav';

import { HomeRoutingModule } from "./home-routing.module";

import { HomeComponent } from "./home-component/home.component";
import { NavbarComponent } from "./home-component/navbar-component/navbar.component";
import { RoomComponent } from "./home-component/rooms-component/room-component/room.component";
import { RoomsComponent } from "./home-component/rooms-component/rooms.component";
import { SensorComponent } from "./home-component/rooms-component/room-component/sensors-component/sensor-component/sensor.component";
import { SensorsComponent } from "./home-component/rooms-component/room-component/sensors-component/sensors.component";
import { SidenavComponent } from "./home-component/sidenav-component/sidenav.component";

@NgModule({
    declarations: [
      HomeComponent,
      NavbarComponent,
      SidenavComponent,
      RoomsComponent,
      RoomComponent,
      SensorsComponent,
      SensorComponent,
    ],
    imports: [
      CommonModule,
      HomeRoutingModule,
      MatToolbarModule,
      MatIconModule,
      MatSidenavModule
    ],
    providers: []
  })
export class HomeModule { }