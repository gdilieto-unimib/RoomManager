import { NgModule } from "@angular/core";
import { CommonModule } from "@angular/common";
import { ReactiveFormsModule } from '@angular/forms';
import { Form } from "./classes/form.class";

@NgModule({
    declarations: [
      Form
    ],
    imports: [
      CommonModule,
      ReactiveFormsModule,
    ],
    providers: []
  })
export class HomeModule { }