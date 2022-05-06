import { NgModule } from "@angular/core";
import { CommonModule } from "@angular/common";
import { ReactiveFormsModule } from '@angular/forms';

import { MatToolbarModule } from '@angular/material/toolbar';
import { MatIconModule } from '@angular/material/icon';
import { MatSidenavModule } from '@angular/material/sidenav';
import { MatGridListModule } from '@angular/material/grid-list';
import { MatButtonModule } from '@angular/material/button';
import { MatCardModule } from '@angular/material/card';
import { MatTabsModule } from '@angular/material/tabs';
import { MatDialogModule } from '@angular/material/dialog';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatInputModule } from '@angular/material/input';

import { HomeRoutingModule } from "./home-routing.module";

import { HomeComponent } from "./home-component/home.component";
import { NavbarComponent } from "./home-component/navbar-component/navbar.component";
import { RoomComponent } from "./home-component/rooms-component/room-component/room.component";
import { RoomsComponent } from "./home-component/rooms-component/rooms.component";
import { SensorComponent } from "./home-component/rooms-component/room-component/sensors-component/sensor-component/sensor.component";
import { SensorsComponent } from "./home-component/rooms-component/room-component/sensors-component/sensors.component";
import { SidenavComponent } from "./home-component/sidenav-component/sidenav.component";
import { EditRoomDialog } from "./home-component/rooms-component/room-component/dialogs/edit-room-dialog/edit-room.dialog";
import { DeleteRoomDialog } from "./home-component/rooms-component/room-component/dialogs/delete-room-dialog/delete-room.dialog";
import { CreateRoomDialog } from "./home-component/rooms-component/room-component/dialogs/create-room-dialog copy/create-room.dialog";

@NgModule({
    declarations: [
      HomeComponent,
      NavbarComponent,
      SidenavComponent,
      RoomsComponent,
      RoomComponent,
      SensorsComponent,
      SensorComponent,
      CreateRoomDialog,
      EditRoomDialog,
      DeleteRoomDialog,
    ],
    imports: [
      CommonModule,
      ReactiveFormsModule,
      HomeRoutingModule,
      MatToolbarModule,
      MatIconModule,
      MatSidenavModule,
      MatGridListModule,
      MatButtonModule,
      MatCardModule,
      MatTabsModule,
      MatDialogModule,
      MatFormFieldModule,
      MatInputModule,
    ],
    providers: []
  })
export class HomeModule { }