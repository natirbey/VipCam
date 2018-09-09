<!-- <div class="k"><input type="button" class="button_my" value="На главную" onClick=""></div>
<div class="k"><input type="button" class="button_my" value="Обновить" onClick="return info('cgi-bin/3ginfo.sh',0);"></div> -->
<span class="l" style="display:{STATUS_SHOW}">Статус:</span><span class="r" style="display:{STATUS_SHOW}">{STATUS}</span>
<span class="l">Сигнал:</span><span class="r"><strong>{CSQ_PER}%</strong></span>
<div style="float:left; margin:1%; width:98%; height:20px; border:1px solid #000000; background-color:transparent;">
 <div style="float:left; background-color:{CSQ_COL}; border-right:1px solid #000000; height:100%; width:{CSQ_PER}%">&nbsp;</div>
</div>
<!-- <div class="c" style="display:{SMS_SHOW};"><input type="button" class="button" value="SMS" onClick="window.location.href='cgi-bin/sms.sh'"></div> -->
<div class="c" style="display:{USSD_SHOW};"><input type="button" class="button" value="USSD" onClick="window.location.href='cgi-bin/ussd.sh'"></div>
<div class="c" style="display:{LIMIT_SHOW};"><input type="button" class="button" value="PozostaÅ‚y limit" onClick="return trzyginfo('cgi-bin/limit.sh',0);"></div>
<!-- <div class="c"><input type="button" class="button" value="Обновить" onClick="return trzyginfo('cgi-bin/3ginfo.sh',0);"></div> -->
<!-- <div class="c" style="display:{STATUS_SHOW}"><input type="button" class="button" value="{STATUS_TRE}" onClick="trzyginfo('cgi-bin/onoff.sh',1);"></div> -->
<!-- <div class="c"><input type="button" class="button" value="Назад" onClick=""></div> -->
<br />
<!--<a href="#" id='details_on' onclick="detailson();" style="float:left;display:inline;"><small>Показать детали</small></a>
<a href="#" id='details_off' onclick="detailsoff();" style="float:left;display:none;"><small>Скрыть детали</small></a>-->
<br />
<!--<div id='details' style="display:none;">-->
<div id='details' style="display:inline;">
<span class="l" style="display:{STATUS_SHOW}"><small>Время с момента соединения:</small></span><span class="r" style="display:{STATUS_SHOW}"><small>{CONN_TIME}</small></span>
<span class="l" style="display:{STATUS_SHOW}"><small>Получено:</small></span><span class="r" style="display:{STATUS_SHOW}"><small>{RX}</small></span>
<span class="l" style="display:{STATUS_SHOW}"><small>Передано:</small></span><span class="r" style="display:{STATUS_SHOW}"><small>{TX}</small></span>
<span class="l">Оператор:</span><span class="r">{COPS}</span>
<span class="l">Режим работы:</span><span class="r">{MODE}</span>
 <span class="l">CSQ:</span><span class="r">{CSQ}</span>
 <span class="l">RSSI:</span><span class="r">{CSQ_RSSI} dBm</span>
 <span class="l" style="display:{QOS_SHOW}">UMTS QoS Profile:</span><span class="r" style="display:{QOS_SHOW}">{DOWN} kbps DOWN | {UP} kbps UP</span>
 <span class="l">MCC MNC:</span><span class="r">{COPS_MCC} {COPS_MNC}</span>
 <span class="l">LAC:</span><span class="r">{LAC} ({LAC_NUM})</span>
 <span class="l" style="display:{LCID_SHOW}">LCID:</span><span class="r" style="display:{LCID_SHOW}">{LCID} ({LCID_NUM})</span>
 <span class="l" style="display:{RNC_SHOW}">RNC:</span><span class="r" style="display:{RNC_SHOW}">{RNC} ({RNC_NUM})</span>
 <span class="l" style="display:{ENB_SHOW}">eNB:</span><span class="r" style="display:{ENB_SHOW}">{ENB} ({ENB_NUM})</span>
 <span class="l">CID:</span><span class="r">{CID} ({CID_NUM})</span>
 <span class="l">&nbsp;</span><small><span class="r">{BTSINFO}</small>
 <span class="l">Устройство:</span><span class="r">{DEVICE}</span>
</div>
