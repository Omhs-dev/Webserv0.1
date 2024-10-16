// CGI
// can have multiple script
// the scenario to use the CGI pending.... ?
// pending...
.
.
.
.
.
.
.
.

// parse the the request
// parse the response
   // while receiving the the request, parse the request or after parsing the req
   // we check what the method of the request (get, post, delete)
   // hadle the request in server side
        // if in the req path "cgi-bin"
            // execute cgi
	            // ex: if req=post -> handlepost
	                // if formal *char
	                    //we handle
	                // if updload file
	                    // we handle
        // else handle request: get -> handleGet, if post handlePost, if delete->handleDelelete
	    // upload a file (method post) -> handlePost
	        // handle the file uploaded
	            // store the file on our root upload folder
	            // the server must know that the file is to be uploade through (multipart/form-data)
		// Delete a file (method delete) -> handleDelete
			// if delete file
				// look for the file path and delete the file
.
.
.
.
.
.
.
// handle the get request method
// 