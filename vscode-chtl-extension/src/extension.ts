import * as vscode from 'vscode';
import * as fs from 'fs';
import * as path from 'path';
import { spawn, exec } from 'child_process';
import * as express from 'express';
import * as WebSocket from 'ws';
import * as chokidar from 'chokidar';

export class CHTLExtension {
    private context: vscode.ExtensionContext;
    private liveServer: express.Application | null = null;
    private webSocketServer: WebSocket.Server | null = null;
    private fileWatcher: chokidar.FSWatcher | null = null;
    private outputChannel: vscode.OutputChannel;
    private compilerPath: string = '';
    private officialModulesPath: string = '';
    private moduleQueryTable: Map<string, any> = new Map();

    constructor(context: vscode.ExtensionContext) {
        this.context = context;
        this.outputChannel = vscode.window.createOutputChannel('CHTL');
        this.initializeExtension();
    }

    private async initializeExtension() {
        this.outputChannel.appendLine('🚀 CHTL Language Support 初始化...');
        
        // 初始化编译器路径
        await this.initializeCompiler();
        
        // 加载官方模块
        await this.loadOfficialModules();
        
        // 创建模块查询表
        await this.createModuleQueryTable();
        
        this.outputChannel.appendLine('✅ CHTL Language Support 初始化完成');
    }

    private async initializeCompiler() {
        // 获取插件内置的编译器路径
        const extensionPath = this.context.extensionPath;
        this.compilerPath = path.join(extensionPath, 'bin', 'chtl_compiler');
        this.officialModulesPath = path.join(extensionPath, 'modules');
        
        // 检查编译器是否存在
        if (!fs.existsSync(this.compilerPath)) {
            vscode.window.showWarningMessage('CHTL编译器未找到，某些功能可能无法使用');
            this.outputChannel.appendLine('⚠️ 编译器路径不存在: ' + this.compilerPath);
        } else {
            this.outputChannel.appendLine('✓ 编译器路径: ' + this.compilerPath);
        }
        
        // 检查官方模块目录
        if (!fs.existsSync(this.officialModulesPath)) {
            this.outputChannel.appendLine('⚠️ 官方模块目录不存在: ' + this.officialModulesPath);
        } else {
            this.outputChannel.appendLine('✓ 官方模块目录: ' + this.officialModulesPath);
        }
    }

    private async loadOfficialModules() {
        this.outputChannel.appendLine('📦 加载官方模块...');
        
        try {
            // 解包CMOD模块（只处理CMOD，不处理CJMOD）
            const modulesDir = this.officialModulesPath;
            
            if (fs.existsSync(modulesDir)) {
                const modules = fs.readdirSync(modulesDir);
                
                for (const module of modules) {
                    const modulePath = path.join(modulesDir, module);
                    
                    if (fs.statSync(modulePath).isDirectory()) {
                        await this.unpackCMODModule(modulePath);
                    }
                }
                
                this.outputChannel.appendLine('✓ 官方模块加载完成');
            }
            
        } catch (error) {
            this.outputChannel.appendLine('❌ 官方模块加载失败: ' + error);
        }
    }

    private async unpackCMODModule(modulePath: string) {
        try {
            // 查找CMOD部分
            const cmodPath = this.findCMODPath(modulePath);
            
            if (cmodPath) {
                const infoFile = path.join(cmodPath, 'info', path.basename(modulePath) + '.chtl');
                
                if (fs.existsSync(infoFile)) {
                    const infoContent = fs.readFileSync(infoFile, 'utf8');
                    const moduleInfo = this.parseModuleInfo(infoContent);
                    
                    this.outputChannel.appendLine(`  ✓ 解包模块: ${moduleInfo.name} v${moduleInfo.version}`);
                    
                    // 解析Export块创建查询表
                    if (moduleInfo.exports) {
                        this.moduleQueryTable.set(moduleInfo.name, moduleInfo.exports);
                    }
                }
            }
            
        } catch (error) {
            this.outputChannel.appendLine(`  ❌ 解包模块失败 ${modulePath}: ${error}`);
        }
    }

    private findCMODPath(modulePath: string): string | null {
        // 查找CMOD文件夹（支持CMOD/cmod/Cmod变体）
        const variants = ['CMOD', 'cmod', 'Cmod'];
        
        for (const variant of variants) {
            const cmodPath = path.join(modulePath, variant);
            if (fs.existsSync(cmodPath)) {
                return cmodPath;
            }
        }
        
        return null;
    }

    private parseModuleInfo(content: string): any {
        const moduleInfo: any = {};
        
        // 解析[Info]块
        const infoMatch = content.match(/\[Info\]\s*\{([^}]*)\}/);
        if (infoMatch) {
            const infoContent = infoMatch[1];
            const fieldRegex = /(\w+)\s*=\s*"([^"]*)"/g;
            let match;
            
            while ((match = fieldRegex.exec(infoContent)) !== null) {
                moduleInfo[match[1]] = match[2];
            }
        }
        
        // 解析[Export]块
        const exportMatch = content.match(/\[Export\]\s*\{([^}]*)\}/);
        if (exportMatch) {
            const exportContent = exportMatch[1];
            const exports: any = {};
            
            const exportLineRegex = /\[(\w+)\]\s*@(\w+)\s+([^;]+);/g;
            let match;
            
            while ((match = exportLineRegex.exec(exportContent)) !== null) {
                const category = match[1];  // Custom, Template等
                const type = match[2];      // Style, Element等
                const items = match[3].split(',').map(item => item.trim());
                
                if (!exports[category]) {
                    exports[category] = {};
                }
                
                exports[category][type] = items;
            }
            
            moduleInfo.exports = exports;
        }
        
        return moduleInfo;
    }

    private async createModuleQueryTable() {
        this.outputChannel.appendLine('🔍 创建模块查询表...');
        
        try {
            // 将查询表保存为JSON文件，便于快速查询
            const queryTablePath = path.join(this.context.globalStoragePath, 'module-query-table.json');
            
            // 确保目录存在
            const dir = path.dirname(queryTablePath);
            if (!fs.existsSync(dir)) {
                fs.mkdirSync(dir, { recursive: true });
            }
            
            const queryTableData = Object.fromEntries(this.moduleQueryTable);
            fs.writeFileSync(queryTablePath, JSON.stringify(queryTableData, null, 2));
            
            this.outputChannel.appendLine('✓ 模块查询表创建完成: ' + queryTablePath);
            
        } catch (error) {
            this.outputChannel.appendLine('❌ 查询表创建失败: ' + error);
        }
    }

    // 编译功能
    public async compileFile(filePath: string): Promise<boolean> {
        return new Promise((resolve, reject) => {
            this.outputChannel.appendLine(`🔧 编译文件: ${filePath}`);
            
            if (!fs.existsSync(this.compilerPath)) {
                vscode.window.showErrorMessage('CHTL编译器未找到');
                resolve(false);
                return;
            }
            
            const outputDir = path.join(path.dirname(filePath), 'dist');
            
            // 确保输出目录存在
            if (!fs.existsSync(outputDir)) {
                fs.mkdirSync(outputDir, { recursive: true });
            }
            
            // 执行编译
            const compileProcess = spawn(this.compilerPath, [filePath, '-o', outputDir], {
                cwd: path.dirname(filePath)
            });
            
            let output = '';
            let errorOutput = '';
            
            compileProcess.stdout.on('data', (data) => {
                output += data.toString();
            });
            
            compileProcess.stderr.on('data', (data) => {
                errorOutput += data.toString();
            });
            
            compileProcess.on('close', (code) => {
                if (code === 0) {
                    this.outputChannel.appendLine('✅ 编译成功');
                    this.outputChannel.appendLine(output);
                    vscode.window.showInformationMessage('CHTL编译成功！');
                    resolve(true);
                } else {
                    this.outputChannel.appendLine('❌ 编译失败');
                    this.outputChannel.appendLine(errorOutput);
                    vscode.window.showErrorMessage('CHTL编译失败: ' + errorOutput);
                    resolve(false);
                }
            });
        });
    }

    // 编译并预览
    public async compileAndPreview(filePath: string) {
        const success = await this.compileFile(filePath);
        
        if (success) {
            const htmlFile = path.join(path.dirname(filePath), 'dist', 'index.html');
            
            if (fs.existsSync(htmlFile)) {
                // 在浏览器中打开
                const uri = vscode.Uri.file(htmlFile);
                await vscode.env.openExternal(uri);
            } else {
                vscode.window.showWarningMessage('编译成功，但未找到生成的HTML文件');
            }
        }
    }

    // Live Server功能
    public async startLiveServer() {
        if (this.liveServer) {
            vscode.window.showInformationMessage('Live Server已经在运行');
            return;
        }
        
        const config = vscode.workspace.getConfiguration('chtl');
        const port = config.get<number>('liveServerPort', 3000);
        
        this.liveServer = express();
        
        // 设置静态文件服务
        const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
        if (workspaceFolder) {
            this.liveServer.use(express.static(workspaceFolder.uri.fsPath));
        }
        
        // 启动服务器
        const server = this.liveServer.listen(port, () => {
            this.outputChannel.appendLine(`🌐 Live Server启动: http://localhost:${port}`);
            vscode.window.showInformationMessage(`Live Server启动: http://localhost:${port}`);
        });
        
        // 启动WebSocket服务器用于实时刷新
        this.webSocketServer = new WebSocket.Server({ server });
        
        this.webSocketServer.on('connection', (ws) => {
            this.outputChannel.appendLine('🔗 客户端连接到Live Server');
        });
        
        // 监听文件变化
        if (workspaceFolder) {
            this.fileWatcher = chokidar.watch('**/*.chtl', {
                cwd: workspaceFolder.uri.fsPath,
                ignored: /node_modules|\.git/
            });
            
            this.fileWatcher.on('change', async (filePath) => {
                this.outputChannel.appendLine(`📝 文件变化: ${filePath}`);
                
                // 自动编译
                const fullPath = path.join(workspaceFolder.uri.fsPath, filePath);
                const success = await this.compileFile(fullPath);
                
                if (success && this.webSocketServer) {
                    // 通知客户端刷新
                    this.webSocketServer.clients.forEach((client) => {
                        if (client.readyState === WebSocket.OPEN) {
                            client.send(JSON.stringify({ type: 'reload' }));
                        }
                    });
                }
            });
        }
    }

    public stopLiveServer() {
        if (this.liveServer) {
            this.liveServer = null;
            this.outputChannel.appendLine('🛑 Live Server已停止');
            vscode.window.showInformationMessage('Live Server已停止');
        }
        
        if (this.webSocketServer) {
            this.webSocketServer.close();
            this.webSocketServer = null;
        }
        
        if (this.fileWatcher) {
            this.fileWatcher.close();
            this.fileWatcher = null;
        }
    }

    // 创建CHTL项目
    public async createProject() {
        const projectName = await vscode.window.showInputBox({
            prompt: '请输入CHTL项目名称',
            placeholder: 'my-chtl-project'
        });
        
        if (!projectName) return;
        
        const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
        if (!workspaceFolder) {
            vscode.window.showErrorMessage('请先打开一个工作区');
            return;
        }
        
        const projectPath = path.join(workspaceFolder.uri.fsPath, projectName);
        
        try {
            // 创建项目结构
            fs.mkdirSync(projectPath, { recursive: true });
            fs.mkdirSync(path.join(projectPath, 'src'), { recursive: true });
            fs.mkdirSync(path.join(projectPath, 'dist'), { recursive: true });
            fs.mkdirSync(path.join(projectPath, 'assets'), { recursive: true });
            
            // 创建基本的CHTL文件
            const indexChtl = `// ${projectName} - CHTL项目
// 由CHTL Language Support插件创建

[Import] @Chtl from chtl::Chtholly
[Import] @Chtl from chtl::Yuigahama

[Configuration]
{
    INDEX_INITIAL_COUNT = 1;
    DEBUG_MODE = false;
}

[Template] @Var ProjectColors
{
    primary: "#3498db";
    secondary: "#2ecc71";
    accent: "#e74c3c";
}

html
{
    lang: "zh-CN";
    
    head
    {
        meta { charset: "UTF-8"; }
        meta { name: "viewport"; content: "width=device-width, initial-scale=1.0"; }
        
        title
        {
            text { "${projectName}" }
        }
    }
    
    body
    {
        header
        {
            style
            {
                header
                {
                    background: "linear-gradient(135deg," ProjectColors(primary) "," ProjectColors(secondary) ")";
                    color: "white";
                    padding: "2rem 0";
                    text-align: "center";
                }
            }
            
            h1
            {
                text { "欢迎来到${projectName}" }
            }
            
            p
            {
                text { "使用CHTL编译器构建的现代Web应用" }
            }
        }
        
        main
        {
            style
            {
                main
                {
                    padding: "3rem 0";
                    max-width: "1200px";
                    margin: "0 auto";
                }
            }
            
            -- 使用Chtholly模块的卡片
            ChthollyCard()
            {
                div[1]
                {
                    h2 { text { "CHTL特性" } }
                    p { text { "体验CHTL编译器的强大功能！" } }
                }
            }
            
            -- 使用Yuigahama模块的音乐播放器
            YuigahaMusicPlayer()
            {
                script
                {
                    {{.player-btn.play}} &-> click
                    {
                        console.log('🎵 音乐播放器启动！');
                    }
                }
            }
        }
        
        script
        {
            console.log('🎉 ${projectName} 加载完成！');
            console.log('✨ 感谢使用CHTL编译器！');
        }
    }
}

use html5;`;
            
            fs.writeFileSync(path.join(projectPath, 'src', 'index.chtl'), indexChtl);
            
            // 创建README
            const readme = `# ${projectName}

使用CHTL编译器创建的项目。

## 开发

1. 编辑 \`src/index.chtl\` 文件
2. 右键选择 "Compile and Preview" 编译并预览
3. 或使用 Ctrl+Shift+P 快捷键

## 功能

- ✅ CHTL语法支持
- ✅ 官方模块集成
- ✅ 实时预览
- ✅ 自动编译

## 官方模块

- 🌸 Chtholly模块：优雅的UI组件
- 💖 Yuigahama模块：温暖的交互体验
`;
            
            fs.writeFileSync(path.join(projectPath, 'README.md'), readme);
            
            // 打开项目文件
            const indexFile = vscode.Uri.file(path.join(projectPath, 'src', 'index.chtl'));
            await vscode.window.showTextDocument(indexFile);
            
            vscode.window.showInformationMessage(`CHTL项目 "${projectName}" 创建成功！`);
            this.outputChannel.appendLine(`✅ 项目创建成功: ${projectPath}`);
            
        } catch (error) {
            vscode.window.showErrorMessage('项目创建失败: ' + error);
            this.outputChannel.appendLine('❌ 项目创建失败: ' + error);
        }
    }

    // 提供自动补全
    public provideCompletionItems(document: vscode.TextDocument, position: vscode.Position): vscode.CompletionItem[] {
        const items: vscode.CompletionItem[] = [];
        
        // CHTL关键字自动补全（注意避免与VSCode []冲突）
        const chtlKeywords = [
            'Template', 'Custom', 'Origin', 'Import', 'Namespace', 'Configuration',
            'text', 'style', 'script', 'inherit', 'delete', 'insert', 'replace',
            'except', 'use', 'from', 'as', 'after', 'before'
        ];
        
        for (const keyword of chtlKeywords) {
            const item = new vscode.CompletionItem(keyword, vscode.CompletionItemKind.Keyword);
            item.detail = 'CHTL关键字';
            item.documentation = `CHTL语法关键字: ${keyword}`;
            items.push(item);
        }
        
        // 官方模块组件自动补全
        for (const [moduleName, moduleExports] of this.moduleQueryTable) {
            if (moduleExports && moduleExports.Custom) {
                for (const [type, components] of Object.entries(moduleExports.Custom)) {
                    if (Array.isArray(components)) {
                        for (const component of components) {
                            const item = new vscode.CompletionItem(component, vscode.CompletionItemKind.Class);
                            item.detail = `${moduleName}模块 - ${type}组件`;
                            item.documentation = `来自${moduleName}官方模块的${type}组件`;
                            item.insertText = `${component}()`;
                            items.push(item);
                        }
                    }
                }
            }
        }
        
        // CHTL JS增强语法自动补全
        const chtlJSItems = [
            { name: 'animate', detail: 'CHTL JS动画块', snippet: 'animate {\n\ttarget: $1,\n\tduration: $2,\n\tbegin: { $3 },\n\tend: { $4 }\n}' },
            { name: 'listen', detail: 'CHTL JS事件监听', snippet: 'listen {\n\t$1: function(e) {\n\t\t$2\n\t}\n}' },
            { name: 'vir', detail: 'CHTL JS虚拟对象', snippet: 'vir $1 = listen {\n\t$2: function() {\n\t\t$3\n\t}\n};' },
            { name: 'delegate', detail: 'CHTL JS事件委托', snippet: 'delegate {\n\ttarget: $1,\n\t$2: function(e) {\n\t\t$3\n\t}\n}' }
        ];
        
        for (const jsItem of chtlJSItems) {
            const item = new vscode.CompletionItem(jsItem.name, vscode.CompletionItemKind.Snippet);
            item.detail = jsItem.detail;
            item.insertText = new vscode.SnippetString(jsItem.snippet);
            items.push(item);
        }
        
        return items;
    }

    public dispose() {
        this.stopLiveServer();
        this.outputChannel.dispose();
    }
}

// 扩展激活函数
export function activate(context: vscode.ExtensionContext) {
    console.log('🚀 CHTL Language Support 扩展激活');
    
    const chtlExtension = new CHTLExtension(context);
    
    // 注册命令
    const compileCommand = vscode.commands.registerCommand('chtl.compile', async (uri: vscode.Uri) => {
        if (uri) {
            await chtlExtension.compileFile(uri.fsPath);
        } else {
            const activeEditor = vscode.window.activeTextEditor;
            if (activeEditor) {
                await chtlExtension.compileFile(activeEditor.document.fileName);
            }
        }
    });
    
    const compileAndPreviewCommand = vscode.commands.registerCommand('chtl.compileAndPreview', async (uri: vscode.Uri) => {
        if (uri) {
            await chtlExtension.compileAndPreview(uri.fsPath);
        } else {
            const activeEditor = vscode.window.activeTextEditor;
            if (activeEditor) {
                await chtlExtension.compileAndPreview(activeEditor.document.fileName);
            }
        }
    });
    
    const startLiveServerCommand = vscode.commands.registerCommand('chtl.startLiveServer', () => {
        chtlExtension.startLiveServer();
    });
    
    const stopLiveServerCommand = vscode.commands.registerCommand('chtl.stopLiveServer', () => {
        chtlExtension.stopLiveServer();
    });
    
    const createProjectCommand = vscode.commands.registerCommand('chtl.createProject', () => {
        chtlExtension.createProject();
    });
    
    const loadOfficialModulesCommand = vscode.commands.registerCommand('chtl.loadOfficialModules', () => {
        vscode.window.showInformationMessage('官方模块已加载：Chtholly, Yuigahama');
    });
    
    // 注册自动补全提供者
    const completionProvider = vscode.languages.registerCompletionItemProvider(
        ['chtl', 'chtl-js'],
        {
            provideCompletionItems(document: vscode.TextDocument, position: vscode.Position) {
                return chtlExtension.provideCompletionItems(document, position);
            }
        },
        // 触发字符（避免与VSCode []冲突）
        '@', '.', '#', '{'
    );
    
    // 注册文件保存时自动编译
    const saveListener = vscode.workspace.onDidSaveTextDocument(async (document) => {
        if (document.languageId === 'chtl') {
            const config = vscode.workspace.getConfiguration('chtl');
            const autoCompile = config.get<boolean>('autoCompileOnSave', true);
            
            if (autoCompile) {
                await chtlExtension.compileFile(document.fileName);
            }
        }
    });
    
    // 添加到上下文
    context.subscriptions.push(
        compileCommand,
        compileAndPreviewCommand,
        startLiveServerCommand,
        stopLiveServerCommand,
        createProjectCommand,
        loadOfficialModulesCommand,
        completionProvider,
        saveListener,
        chtlExtension
    );
}

export function deactivate() {
    console.log('CHTL Language Support 扩展已停用');
}