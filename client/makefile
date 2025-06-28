CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude # 编译器的通用标志

# 链接器标志：只包含库链接
LDFLAGS = -lboost_json 

# 源文件和对象文件
SRCS = src/construct.cpp src/login.cpp main.cpp
OBJS = $(patsubst src/%.cpp,obj/%.o,$(filter src/%.cpp,$(SRCS)))
# 注意：main.cpp 通常不会被单独编译成 .o，而是直接在链接阶段使用。
# 更好的做法是把 main.cpp 也编译成 .o 文件，然后一起链接。
# 假设你的 makefile 已经处理了 main.cpp

# 目标文件目录
OBJDIR = obj

# 默认目标
all: client

# 链接可执行文件
client: $(OBJS) $(OBJDIR)/main.o # 假设main.cpp也编译成了obj/main.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) # $^ 表示所有依赖文件

# 编译 .cpp 文件为 .o 文件
$(OBJDIR)/%.o: src/%.cpp
	@mkdir -p $(OBJDIR) # 确保obj目录存在
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 如果 main.cpp 不编译成 .o，则链接规则可能像这样：
# client: obj/construct.o obj/login.o
# 	$(CXX) $(CXXFLAGS) obj/construct.o obj/login.o main.cpp -o client $(LDFLAGS)


.PHONY: clean
clean:
	rm -f $(OBJS) $(OBJDIR)/*.o client