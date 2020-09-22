/*Downloaded from https://www.codeseek.co/paulmerupu91/wifi-signal-css-illustration-LevMgR */
$(document).ready(function(){
    $(".clickHere").click(function(){
      
      $(".signal div").css("opacity","1");
      $(".signal div").show();
      
      var level = $(this).html();
      
      
      switch (level) {
          
        case "high":
          
            $(".curveOne .med").show();
            //$("body").animate({backgroundColor:"#e03333"},1500);
            $("body").css("background","#d1512e");
          
          break;
          
        case "med":
          
            $(".curveOne").hide();
            $(".curveFour").show();
            $("body").css("background","#ed7e38");
            //$("body").animate({backgroundColor:"#ed7e38"},1500);
         
          break;
          
        case "low":
          
            $(".med").hide();
            $(".curveOne").hide();
            $(".curveFour").show();
            $("body").css("background","#edaa53");
            //$("body").animate({backgroundColor:"#edaa53"},1500);
            
          
          break;
          
        case "off":
          
          $(".signal div").css("opacity",".3");
          $("body").css("background","linear-gradient(5deg, #66c5dd, #b6bfef)");
          break;
          
       
          
        
      }
      
      
//           if($(this).html() == "med"){
//             $(".curveOne").hide();
//             //alert($(this).html());
//           }
      
//           if($(this).html() == "low"){
//             $(".med").hide();
//             $(".curveOne").hide();
//           } else{
//             //$(".med").show();
//             $(".curveOne").show();
//           }
      
//           if($(this).html() == "high"){
//             $(".curveOne .med").show();
//           }
        
          $(this).addClass( "clickHere click" );
          $(this).siblings().removeClass("click");
          
          
      
    });
  
  
  
  
});