-- Game global script

G.GameScript = LOCAL;              -- setup a global link to our local variables

-- configure the message system
VisionMessage.SetMaxNum(40)
VisionMessage.SetDuration(20)

-- insert a space
VisionMessage.Add("");

-- print a message
VisionMessage.Add("Running Game Script");

G.message = "Hello Global World from Game";  -- setup a global variable

-- script-local variable
message = "Hello Local World from Game";

