module Method = H2.Method;

module Status = H2.Status;

module Client = {
  module Request = {
    type t = {
      meth: Method.t,
      uri: Uri.t,
      body: option(string),
      headers: list((string, string)),
    };

    let body = t => t.body;
    let meth = t => t.meth;
    let uri = t => t.uri;
    let path = t => t.uri |> Uri.path_and_query
    let headers = t => t.headers;
    let host = t => t.uri |> Uri.host_with_default;
    let scheme = t =>
      switch (t |> uri |> Uri.scheme) {
      | None => ""
      | Some(s) => s
      };

    let create = (~headers=[], ~body="", meth, uri) => {
      let host = Uri.host_with_default(uri);
      let content_length = body |> String.length |> string_of_int;
      let headers =
        [("Host", host), ("Content-Length", content_length)] @ headers;
      let body =
        switch (body) {
        | "" => None
        | _ => Some(body)
        };

      {meth, uri, headers, body};
    };

    let to_string = req => {
      (req.meth |> H2.Method.to_string)
      ++ " "
      ++ (req.uri |> Uri.path)
      ++ "\n"
      ++ (req.headers |> H2.Headers.of_list |> H2.Headers.to_string)
      ++ (
        switch (req.body) {
        | None => ""
        | Some(s) => "\n\n" ++ s
        }
      );
    };
  };
};
