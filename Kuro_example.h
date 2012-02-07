void LCDPrint(String Message, int X, int Y) {
	char LCDString[16];

	#Fill with Blanks
	for (int i=0;i<=16i++) { LCDString[i]=" "; }

	#If X or Y set change position
	if ((X>0) || (Y>0)) { slcd.setCursor(X,Y); }

	#Check Message is 16chars long, if longer ignore excess, if 0 (empty) do nothing.
	int MessageSize = Message.size();
	if (MessageSize >= 1) {
		if (MessageSize > 16) { MessageSize = 16; }
		for (int i=0;i<MessageSize;i++) {LCDString[i]=Message[i] }
	}

	#convert back to string
	s = "";
	for (int i=0;i<16;i++) { s+=LCDString[i]; }

	#print new string
	slcd.print(LCDString);
}