function showHide(shID) 
{
   if (document.getElementById(shID+'-short')) 
   {
      if (document.getElementById(shID+'-short').style.display == 'none') 
      {
         document.getElementById(shID+'-long').style.display = 'none';
         document.getElementById(shID+'-short').style.display = 'block';
      }
      else 
      {
         document.getElementById(shID+'-long').style.display = 'block';
         document.getElementById(shID+'-short').style.display = 'none';
      }
   }
}
