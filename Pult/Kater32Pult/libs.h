

void scan_controls() {
  adc_rul = analogRead(RUL_PIN); // 8 mks один замер
  adc_gaz = analogRead(GAZ_PIN);
}


void send_paket() {
  ctrl.rul = map(adc_rul, 0, 4096, -127, 128);
  ctrl.gaz = map(adc_gaz, 0, 4096, -127, 128);
  //DBG.print(" sat ");DBG.print(tlm.gps.sat.cnt);
  //DBG.print(" rul ");DBG.print(ctrl.rul);DBG.print(" gaz ");DBG.println(ctrl.gaz);
  Pult.sendData();
}

