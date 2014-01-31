var labelType, useGradients, nativeTextSupport, animate;

(function() {
  var ua = navigator.userAgent,
      iStuff = ua.match(/iPhone/i) || ua.match(/iPad/i),
      typeOfCanvas = typeof HTMLCanvasElement,
      nativeCanvasSupport = (typeOfCanvas == 'object' || typeOfCanvas == 'function'),
      textSupport = nativeCanvasSupport 
        && (typeof document.createElement('canvas').getContext('2d').fillText == 'function');
  //I'm setting this based on the fact that ExCanvas provides text support for IE
  //and that as of today iPhone/iPad current text support is lame
  labelType = (!nativeCanvasSupport || (textSupport && !iStuff))? 'Native' : 'HTML';
  nativeTextSupport = labelType == 'Native';
  useGradients = nativeCanvasSupport;
  animate = !(iStuff || !nativeCanvasSupport);
})();


function init(items)
{
    for(var loopIndex=0;loopIndex<items;loopIndex++)
    {
        var infovis = document.getElementById('infovis'+loopIndex);
        
        var areaChart = new $jit.AreaChart({
          injectInto: 'infovis'+loopIndex,
          animate: true,
          Margin: 
          {  top: 5, left: 5, right: 5, bottom: 5 },
          labelOffset: 10,
          showAggregates: true,
          showLabels: true,
          type: useGradients ? 'stacked:gradient' : 'stacked',
          Label: {
            type: labelType,
            size: 10,
            family: 'Arial',
            color: 'black'
          },
          Tips: {
            enable: true,
            onShow: function(tip, elem) {
              tip.innerHTML = "<b>" + elem.name + "</b>: " + elem.value;
            }
          },
          filterOnClick: false,
          restoreOnRightClick:false
        });        
        areaChart.loadJSON(json[loopIndex]);
    }
}
