import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { HomeComponent } from './home-component/home.component';
import { RoomsComponent } from './home-component/rooms-component/rooms.component';

const routes: Routes = [
  {path: '', component: HomeComponent, children: [
    {path: '', component: RoomsComponent},
  ]},
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class HomeRoutingModule { }
