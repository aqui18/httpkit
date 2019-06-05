/** Handle sigpipe internally */
Sys.(set_signal(sigpipe, Signal_ignore));

/** Setup loggers */
Fmt_tty.setup_std_outputs();
Logs.set_level(Some(Logs.Debug));
Logs.set_reporter(Logs_fmt.reporter());

open Httpkit;

module App = {
  type state = {req_count: int};
  let initial_state = {req_count: 0};
  type other = {
    req_count: int,
    name: string,
  };
  let inc: Server.middleware(state, other) =
    ctx => {req_count: ctx.state.req_count + 1, name: "what"};
  let json: Server.middleware(other, unit) =
    ctx => {
      let str = ctx.state.req_count |> string_of_int;
      ctx.respond(~status=`OK, str);
    };
};

let on_start = () => Logs.app(m => m("Running on localhost:9999"));

Httpkit.Server.(
  make(App.initial_state)
  |> use(Httpkit.Middleware.Common.log)
  |> use(App.inc)
  |> reply(App.json)
  |> Httpkit_lwt.Server.Http.listen(
       ~address=`Of_string("hello"),
       ~port=9999,
       ~on_start,
     )
  |> Httpkit_lwt.Https.listen(~port=9999, ~on_start, ~key, ~cert)
  |> Lwt_main.run
);
