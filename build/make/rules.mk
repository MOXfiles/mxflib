

clean:
	rm -f $(PREREQDIR)/*.d
	rm -f $(CPREREQDIR)/*.d
	rm -f $(OBJS)
	rm -f $(OBJSDIR)/*.o
	rm -f $(COBJSDIR)/*.o
	rm -f $(TARGET)

release:
	make RELEASE=1


$(COBJSDIR)/marker: 
	mkdir -p $(COBJSDIR)
	touch $(COBJSDIR)/marker

$(OBJSDIR)/marker: 
	mkdir -p $(OBJSDIR)
	touch $(OBJSDIR)/marker

$(PREREQDIR)/marker: 
	mkdir -p $(PREREQDIR)
	touch $(PREREQDIR)/marker

$(CPREREQDIR)/marker: 
	mkdir -p $(CPREREQDIR)
	touch $(CPREREQDIR)/marker

$(TARGETDIR)/marker: 
	mkdir -p $(TARGETDIR)
	touch $(TARGETDIR)/marker

$(OBJSDIR)/%.o : %.cpp $(PREREQDIR)/%.d 
	$(CC) -c -o $@  $(CXXFLAGS) $(INCLUDE) $<


$(COBJSDIR)/%.o : %.c $(CPREREQDIR)/%.d 
	$(CC) -c -o $@  $(CXXFLAGS) $(INCLUDE) $<

$(PREREQDIR)/%.d : %.cpp Makefile
	rm -f $@; \
	$(CC) -MM $(INCLUDE) $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(PREREQDIR)/%.d : %.cpp makefile
	rm -f $@; \
	$(CC) -MM $(INCLUDE) $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(CPREREQDIR)/%.d : %.c Makefile
	rm -f $@; \
	$(CC) -MM $(INCLUDE) $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(CPREREQDIR)/%.d : %.c makefile
	rm -f $@; \
	$(CC) -MM $(INCLUDE) $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.PRECIOUS : $(PREREQDIR)/%.d

-include $(PREREQDIR)/*.d

