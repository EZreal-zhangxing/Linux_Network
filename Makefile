
OBJ = src/cal.o src/add/add_float.o src/add/add_int.o src/sub/sub_float.o src/sub/sub_int.o 
TARGET = cal
INCLUDE = -Iadd_int -Iadd_float -Isub_int -Isub_float
$(TARGET):$(OBJ)
	g++ $^ -o $@ $(INCLUDE)

# 自动规则推导
$(OBJ):%o:%cpp
	g++ $< -c -o $@ $(INCLUDE)
clean:
	-$(RM) -f $(OBJ)
	-$(RM) -f $(TARGET)