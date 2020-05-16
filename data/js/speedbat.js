// Or with jQuery
var g1, g3;

var g1 = new JustGage({
    id: "g1",
    value: 0 ,
    min: 0,
    max: 150,
    title: "Speed",
    label: "Speed",
    relativeGaugeSize: true,
    donut: true,
    valueFontColor: "white",
   
});



var g3 = new JustGage({
    id: "g3",
    value: getRandomInt(0, 100),
    min: 0,
    max: 100,
    title: "Battery",
    label: "Battery",
    relativeGaugeSize: true,
    donut: true,
    valueFontColor: "white",
   
});


    var els = {
        position : {
            x : "x" ,
            y: "y"
        },
        force : "force",
        pressure : "pressure",
        distance : "distance",
        angle : {
            radian : "radian",
            degree : "degree"
        },
        direction: {
            x : "x",
            y : "y",
            angle :"angle"
        },
        type : "type"
    };
    var joystick = nipplejs.create({
        zone: document.getElementById('joystick'),
        mode: 'static',
        
        position: {
          left: '50%',
          top: '50%'
        },
        color: '#00F0FF',
        size: 300
      });

      bindNipple ();

      
    var url = "joydata" ;

    function bindNipple () {
        joystick.on('start end', function (evt, data) {
            debug(data);
            dump(evt);
      if (evt.type == 'end'){

        g1.refresh(0); 
      }

        }).on('move', function (evt, data) {
            debug(data);
            dump(evt);

         g1.refresh(data.distance);    
        
        });
    }
    
    // Print data into elements
    function debug (obj) {
        function parseObj(sub, el) {
            for (var i in sub) {
                //var mmm = sub[i];
                //console.log (mmm);
                if (typeof sub[i] === 'object' && el) {
                    parseObj(sub[i], el[i]);
                } else if (el && el[i]) {
                    el[i] = sub[i];
                }
            }
        }
       
            parseObj(obj, els);
        ;
        post(url, els, "POST", drive);

        console.log(els);
    }

    

    // Dump data
    function dump (evt) {
        setTimeout(function () {
           
            function parseObj(sub, el) {
                for (var i in sub) {

                    if (typeof sub[i] === 'object' && el) {
                        parseObj(sub[i], el[i]);
                    } else if (el && el[i]) {
                        el[i] = sub[i];
                    }
                }
            }

            parseObj(evt, els);

            post(url, els, "POST", drive);
            console.log(els.type);
           //console.log(evt);
            
        });
    }

    function post(target, messageObject, type, handler) {
        let jsonMsg = JSON.stringify(messageObject);
        $.ajax({
          type: type,
          url: "/api/" + target,
          dataType: 'json',
          contentType: 'application/json',
          async: true,
          data: jsonMsg,
          success: function (json) {
            if (handler)
              handler(json);
          }
        });
      }
    
      function drive(json) {
        
        console.log(json.info);
        console.log(json.speed);
        
        $('#drivestat').text(json.info);

        //g1.refresh(json.speed);
        
      }
    