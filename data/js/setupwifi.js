
  // 'Arduino'-variables in this state object
  var state = {};

  // Start wifi scan
  post("access-points", {}, "GET", setAccessPoints);


  


  function selectWifi(i) {
   
    
    
   
    let newLocal = '<h6 id="connt">connect to ' + state.accessPoints[i].SSID + ' </h6><div class="input-field"></div> <input id="pass" type="password" class="validate"><label for="pass">Password</label>';
    $('.modal .modal-content' ).html(newLocal);

   // $('.modal').destroy();
   
    //$('.modal').append(newLocal);
    
   
    
   // $('#modalpass .modal .modal-content').text("Connect to " + state.accessPoints[i].SSID);
   // $('#modalpass .modal-body').html('<input id="password" type="password" class="form-control" placeholder="Password" aria-label="Password">');

   // $('#dismiss_button').text("Cancel");
    //$('#confirm_button').text("Connect");
   // $('#confirm_button').show();
    //$("#confirm_button").unbind();

    $("#confirm_button").click(function (event) {
      let password = $("#pass").val();
      connectToWifi(state.accessPoints[i].SSID, password);
      console.log("Connecting to " + state.accessPoints[i].SSID);
    });

    //$('.modal').modal();
  }

  function connectToWifi(SSID, password) {
    post("set-wifi", { "SSID": SSID, "password": password }, "POST", connected);
  }

  function connected(json) {
    console.log(json);
    $('#connt .modal .modal-content').text(json.info);

   
   
   // $('.modal').modal();
  }

  function setAccessPoints(json) {
    state.accessPoints = json.accessPoints;

    $("#accessPoints").empty();
    for (let i = 0; i < json.accessPoints.length; i++) {
      let SSID = json.accessPoints[i].SSID;
      console.log(SSID);
      let encryption = json.accessPoints[i].encryption;

     
      $("#accessPoints").html(`<div class="card blue-grey darken-1"><a id="selectWifi${i}"  class="btn-floating halfway-fab modal-trigger waves-effect waves-light red" href="#modalpass" ><i class="material-icons">wifi</i></a><div class="card-content white-text"> <span class="card-title">${SSID}</span></div> </div> `);

      //$("#accessPoints").append(`<div id="selectWifi${i}" class="wifi-card btn card mb-2 card-body d-flex flex-row"> <h4 class="card-title">${SSID}</h4> </div>`);
      $("#selectWifi" + i).click(function (event) {
        event.preventDefault();
        selectWifi(i);
      });
    }
  }

  /** GET/POST json message to server */
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