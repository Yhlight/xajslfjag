
// 全局工具函数
window.CHTLUtils = {
    // 防抖函数
    debounce: function(func, wait) {
        let timeout;
        return function executedFunction(...args) {
            const later = () => {
                clearTimeout(timeout);
                func(...args);
            };
            clearTimeout(timeout);
            timeout = setTimeout(later, wait);
        };
    },
    
    // 节流函数
    throttle: function(func, limit) {
        let inThrottle;
        return function() {
            const args = arguments;
            const context = this;
            if (!inThrottle) {
                func.apply(context, args);
                inThrottle = true;
                setTimeout(() => inThrottle = false, limit);
            }
        }
    },
    
    // 格式化日期
    formatDate: function(date) {
        return new Intl.DateTimeFormat('zh-CN', {
            year: 'numeric',
            month: 'long',
            day: 'numeric',
            hour: '2-digit',
            minute: '2-digit'
        }).format(date);
    },
    
    // 生成UUID
    generateUUID: function() {
        return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function(c) {
            const r = Math.random() * 16 | 0;
            const v = c == 'x' ? r : (r & 0x3 | 0x8);
            return v.toString(16);
        });
    }
};

// CHTL JS模块加载（AMD风格）
(function() {
    const loadedModules = [
        './modules/navigation.cjjs',
        './modules/smooth-scroll.cjjs',
        './modules/theme-system.cjjs',
        './modules/performance-monitor.cjjs',
        './modules/analytics.cjjs',
        './utils/dom-helpers.js'
    ];
    
    loadedModules.forEach(function(modulePath) {
        const script = document.createElement('script');
        script.src = modulePath;
        script.async = true;
        document.head.appendChild(script);
    });
    
    console.log('CHTL JS模块加载完成:', loadedModules.length, '个模块');
})();

// 导航控制器（虚拟对象）
var navigationController = {
    setActiveLink: function(href) {
        document.querySelectorAll('.nav-link').forEach(link => {
            link.classList.toggle('active', link.getAttribute('href') === href);
        });
    },
    
    getCurrentSection: function() {
        const sections = document.querySelectorAll('section[id]');
        const scrollTop = window.pageYOffset + 100;
        
        for (let section of sections) {
            if (section.offsetTop <= scrollTop && 
                section.offsetTop + section.offsetHeight > scrollTop) {
                return '#' + section.id;
            }
        }
        return '#home';
    }
};

// 导航链接点击事件
document.querySelectorAll('.nav-link').forEach(function(link) {
    link.addEventListener('click', function(e) {
        e.preventDefault();
        
        // 移除所有active类
        document.querySelectorAll('.nav-link').forEach(l => {
            l.classList.remove('active');
        });
        
        // 添加active类到当前链接
        this.classList.add('active');
        
        // 平滑滚动到目标
        const target = this.getAttribute('href');
        if (target.startsWith('#')) {
            const element = document.querySelector(target);
            if (element) {
                // CHTL JS动画实现
                const startY = window.pageYOffset;
                const targetY = element.offsetTop - 80;
                const distance = targetY - startY;
                const duration = 800;
                const startTime = performance.now();
                
                function animateScroll(currentTime) {
                    const elapsed = currentTime - startTime;
                    const progress = Math.min(elapsed / duration, 1);
                    
                    // 缓动函数：ease-in-out
                    const easeInOut = progress < 0.5 
                        ? 2 * progress * progress 
                        : 1 - Math.pow(-2 * progress + 2, 2) / 2;
                    
                    window.scrollTo(0, startY + distance * easeInOut);
                    
                    if (progress < 1) {
                        requestAnimationFrame(animateScroll);
                    }
                }
                
                requestAnimationFrame(animateScroll);
            }
        }
    });
});

// 滚动时更新导航状态
window.addEventListener('scroll', CHTLUtils.throttle(function() {
    const currentSection = navigationController.getCurrentSection();
    navigationController.setActiveLink(currentSection);
}, 100));

// 产品展示交互
document.querySelectorAll('.product-showcase').forEach(function(showcase) {
    showcase.addEventListener('mouseenter', function() {
        const icon = this.querySelector('.product-icon');
        
        if (icon) {
            // CHTL JS动画：图标旋转缩放
            const startTime = performance.now();
            const duration = 400;
            
            function animateIcon(currentTime) {
                const elapsed = currentTime - startTime;
                const progress = Math.min(elapsed / duration, 1);
                
                const scale = 1 + 0.1 * Math.sin(progress * Math.PI);
                const rotation = progress * 360 * 0.1;
                
                icon.style.transform = `scale(${scale}) rotate(${rotation}deg)`;
                
                if (progress < 1) {
                    requestAnimationFrame(animateIcon);
                }
            }
            
            requestAnimationFrame(animateIcon);
        }
    });
    
    showcase.addEventListener('mouseleave', function() {
        const icon = this.querySelector('.product-icon');
        
        if (icon) {
            // 重置动画
            const startTime = performance.now();
            const duration = 300;
            
            function resetIcon(currentTime) {
                const elapsed = currentTime - startTime;
                const progress = Math.min(elapsed / duration, 1);
                
                const scale = 1.1 - 0.1 * progress;
                const rotation = 36 - 36 * progress;
                
                icon.style.transform = `scale(${scale}) rotate(${rotation}deg)`;
                
                if (progress < 1) {
                    requestAnimationFrame(resetIcon);
                } else {
                    icon.style.transform = 'scale(1) rotate(0deg)';
                }
            }
            
            requestAnimationFrame(resetIcon);
        }
    });
});

// 表单验证器（虚拟对象）
var formValidator = {
    validateName: function(name) {
        if (!name || name.trim().length < 2) {
            return "姓名至少需要2个字符";
        }
        if (name.trim().length > 50) {
            return "姓名不能超过50个字符";
        }
        return null;
    },
    
    validateEmail: function(email) {
        const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
        if (!email || !emailRegex.test(email)) {
            return "请输入有效的邮箱地址";
        }
        return null;
    },
    
    validatePhone: function(phone) {
        if (phone && phone.trim()) {
            const phoneRegex = /^[\+]?[1-9][\d]{0,15}$/;
            if (!phoneRegex.test(phone.replace(/[\s\-\(\)]/g, ''))) {
                return "请输入有效的电话号码";
            }
        }
        return null;
    },
    
    validateMessage: function(message) {
        if (!message || message.trim().length < 10) {
            return "消息内容至少需要10个字符";
        }
        if (message.trim().length > 1000) {
            return "消息内容不能超过1000个字符";
        }
        return null;
    },
    
    showError: function(fieldId, message) {
        const field = document.getElementById(fieldId);
        const errorDiv = document.getElementById(fieldId + '-error');
        
        if (field && errorDiv) {
            field.classList.add('error');
            errorDiv.textContent = message;
            errorDiv.style.display = 'block';
        }
    },
    
    clearError: function(fieldId) {
        const field = document.getElementById(fieldId);
        const errorDiv = document.getElementById(fieldId + '-error');
        
        if (field && errorDiv) {
            field.classList.remove('error');
            errorDiv.style.display = 'none';
        }
    },
    
    validateForm: function() {
        const formData = {
            fullName: document.getElementById('fullName')?.value || '',
            email: document.getElementById('email')?.value || '',
            phone: document.getElementById('phone')?.value || '',
            subject: document.getElementById('subject')?.value || '',
            message: document.getElementById('message')?.value || ''
        };
        
        let isValid = true;
        
        // 清除所有错误
        ['fullName', 'email', 'phone', 'subject', 'message'].forEach(field => {
            this.clearError(field);
        });
        
        // 验证各字段
        const nameError = this.validateName(formData.fullName);
        if (nameError) {
            this.showError('fullName', nameError);
            isValid = false;
        }
        
        const emailError = this.validateEmail(formData.email);
        if (emailError) {
            this.showError('email', emailError);
            isValid = false;
        }
        
        const phoneError = this.validatePhone(formData.phone);
        if (phoneError) {
            this.showError('phone', phoneError);
            isValid = false;
        }
        
        if (!formData.subject) {
            this.showError('subject', '请选择主题');
            isValid = false;
        }
        
        const messageError = this.validateMessage(formData.message);
        if (messageError) {
            this.showError('message', messageError);
            isValid = false;
        }
        
        return { isValid, formData };
    }
};

// 表单实时验证
document.querySelectorAll('.form-input').forEach(function(input) {
    input.addEventListener('blur', function() {
        const fieldId = this.id;
        const value = this.value;
        
        switch (fieldId) {
            case 'fullName':
                const nameError = formValidator.validateName(value);
                if (nameError) {
                    formValidator.showError(fieldId, nameError);
                } else {
                    formValidator.clearError(fieldId);
                }
                break;
                
            case 'email':
                const emailError = formValidator.validateEmail(value);
                if (emailError) {
                    formValidator.showError(fieldId, emailError);
                } else {
                    formValidator.clearError(fieldId);
                }
                break;
                
            case 'phone':
                const phoneError = formValidator.validatePhone(value);
                if (phoneError) {
                    formValidator.showError(fieldId, phoneError);
                } else {
                    formValidator.clearError(fieldId);
                }
                break;
                
            case 'message':
                const messageError = formValidator.validateMessage(value);
                if (messageError) {
                    formValidator.showError(fieldId, messageError);
                } else {
                    formValidator.clearError(fieldId);
                }
                break;
        }
    });
});

// 表单提交处理
document.querySelector('.contact-form')?.addEventListener('submit', function(e) {
    e.preventDefault();
    
    const validation = formValidator.validateForm();
    
    if (validation.isValid) {
        const submitBtn = document.querySelector('.btn-submit');
        const originalText = submitBtn.textContent;
        
        // 显示加载状态
        submitBtn.disabled = true;
        submitBtn.classList.add('loading');
        submitBtn.textContent = '发送中...';
        
        // 模拟提交过程
        setTimeout(() => {
            // 成功动画
            const form = this;
            const startTime = performance.now();
            const duration = 500;
            
            function fadeOut(currentTime) {
                const elapsed = currentTime - startTime;
                const progress = Math.min(elapsed / duration, 1);
                
                form.style.opacity = 1 - progress * 0.2;
                form.style.transform = `scale(${1 - progress * 0.02})`;
                
                if (progress < 1) {
                    requestAnimationFrame(fadeOut);
                } else {
                    // 显示成功消息
                    form.innerHTML = `
                        <div style="text-align: center; padding: 2rem;">
                            <div style="font-size: 48px; margin-bottom: 1rem;">✅</div>
                            <h3 style="color: var(--color-success); margin-bottom: 0.5rem;">发送成功！</h3>
                            <p style="color: var(--color-text-muted);">我们已收到您的消息，会尽快回复。</p>
                        </div>
                    `;
                    
                    // 淡入动画
                    const successStartTime = performance.now();
                    function fadeIn(currentTime) {
                        const elapsed = currentTime - successStartTime;
                        const progress = Math.min(elapsed / 300, 1);
                        
                        form.style.opacity = 0.8 + progress * 0.2;
                        form.style.transform = `scale(${0.98 + progress * 0.02})`;
                        
                        if (progress < 1) {
                            requestAnimationFrame(fadeIn);
                        }
                    }
                    requestAnimationFrame(fadeIn);
                }
            }
            
            requestAnimationFrame(fadeOut);
        }, 2000);
    }
});

// 重置按钮
document.querySelector('.btn-reset')?.addEventListener('click', function() {
    // 重置表单
    document.querySelector('.contact-form')?.reset();
    
    // 清除所有错误
    ['fullName', 'email', 'phone', 'subject', 'message'].forEach(field => {
        formValidator.clearError(field);
    });
    
    // 重置动画
    const form = document.querySelector('.contact-form');
    if (form) {
        const startTime = performance.now();
        const duration = 200;
        
        function resetAnimation(currentTime) {
            const elapsed = currentTime - startTime;
            const progress = Math.min(elapsed / duration, 1);
            
            if (progress < 0.5) {
                form.style.opacity = 1 - progress * 0.5;
            } else {
                form.style.opacity = 0.5 + (progress - 0.5) * 0.5;
            }
            
            if (progress < 1) {
                requestAnimationFrame(resetAnimation);
            } else {
                form.style.opacity = 1;
            }
        }
        
        requestAnimationFrame(resetAnimation);
    }
});

// 全局应用管理器（虚拟对象）
var appManager = {
    currentTheme: "light",
    
    initializeApp: function() {
        console.log('CHTL开发者项目初始化');
        
        // 检测系统主题偏好
        if (window.matchMedia && window.matchMedia('(prefers-color-scheme: dark)').matches) {
            this.switchTheme('dark');
        }
        
        // 添加性能监控
        this.startPerformanceMonitoring();
        
        // 添加错误监控
        window.addEventListener('error', (e) => {
            console.error('JavaScript错误:', e.error);
            this.reportError(e.error.message, e.filename, e.lineno);
        });
        
        // 添加未处理的Promise拒绝监控
        window.addEventListener('unhandledrejection', (e) => {
            console.error('未处理的Promise拒绝:', e.reason);
            this.reportError('Promise rejection: ' + e.reason);
        });
    },
    
    switchTheme: function(theme) {
        this.currentTheme = theme;
        document.documentElement.setAttribute('data-theme', theme);
        
        const themeToggle = document.querySelector('.theme-toggle');
        if (themeToggle) {
            themeToggle.textContent = theme === 'dark' ? '☀️' : '🌙';
        }
        
        // 保存主题偏好
        localStorage.setItem('chtl-theme', theme);
        
        console.log(`主题切换为: ${theme}`);
    },
    
    startPerformanceMonitoring: function() {
        // 性能监控
        if ('PerformanceObserver' in window) {
            const observer = new PerformanceObserver((list) => {
                list.getEntries().forEach((entry) => {
                    if (entry.entryType === 'navigation') {
                        console.log(`页面加载时间: ${entry.loadEventEnd - entry.loadEventStart}ms`);
                    }
                });
            });
            
            observer.observe({ entryTypes: ['navigation', 'measure'] });
        }
        
        // 内存监控
        if (performance.memory) {
            setInterval(() => {
                const memory = performance.memory;
                const used = Math.round(memory.usedJSHeapSize / 1024 / 1024);
                const total = Math.round(memory.totalJSHeapSize / 1024 / 1024);
                console.log(`内存使用: ${used}MB / ${total}MB`);
            }, 60000);
        }
    },
    
    reportError: function(message, filename = '', lineno = 0) {
        // 错误报告
        const errorData = {
            message: message,
            filename: filename,
            lineno: lineno,
            timestamp: new Date().toISOString(),
            userAgent: navigator.userAgent,
            url: window.location.href
        };
        
        console.log('错误报告:', errorData);
        
        // 这里可以发送到错误收集服务
        // fetch('/api/errors', { method: 'POST', body: JSON.stringify(errorData) });
    }
};

// 主题切换按钮
document.querySelector('.theme-toggle')?.addEventListener('click', function() {
    const newTheme = appManager.currentTheme === 'light' ? 'dark' : 'light';
    appManager.switchTheme(newTheme);
    
    // 切换动画
    const button = this;
    const startTime = performance.now();
    const duration = 300;
    
    function rotateButton(currentTime) {
        const elapsed = currentTime - startTime;
        const progress = Math.min(elapsed / duration, 1);
        
        const rotation = progress * 180;
        const scale = 1 + 0.1 * Math.sin(progress * Math.PI);
        
        button.style.transform = `rotate(${rotation}deg) scale(${scale})`;
        
        if (progress < 1) {
            requestAnimationFrame(rotateButton);
        } else {
            // 重置动画
            const resetStartTime = performance.now();
            function resetButton(currentTime) {
                const elapsed = currentTime - resetStartTime;
                const progress = Math.min(elapsed / 200, 1);
                
                const rotation = 180 - 180 * progress;
                const scale = 1.1 - 0.1 * progress;
                
                button.style.transform = `rotate(${rotation}deg) scale(${scale})`;
                
                if (progress < 1) {
                    requestAnimationFrame(resetButton);
                } else {
                    button.style.transform = 'rotate(0deg) scale(1)';
                }
            }
            requestAnimationFrame(resetButton);
        }
    }
    
    requestAnimationFrame(rotateButton);
});

// 移动端菜单切换
document.querySelector('.menu-toggle')?.addEventListener('click', function() {
    const nav = document.querySelector('.main-nav');
    const isVisible = nav && nav.style.display !== 'none';
    
    if (nav) {
        if (isVisible) {
            // 隐藏动画
            const startTime = performance.now();
            const duration = 300;
            
            function hideNav(currentTime) {
                const elapsed = currentTime - startTime;
                const progress = Math.min(elapsed / duration, 1);
                
                nav.style.opacity = 1 - progress;
                nav.style.transform = `translateY(${-20 * progress}px)`;
                
                if (progress < 1) {
                    requestAnimationFrame(hideNav);
                } else {
                    nav.style.display = 'none';
                }
            }
            
            requestAnimationFrame(hideNav);
        } else {
            // 显示动画
            nav.style.display = 'flex';
            const startTime = performance.now();
            const duration = 300;
            
            function showNav(currentTime) {
                const elapsed = currentTime - startTime;
                const progress = Math.min(elapsed / duration, 1);
                
                nav.style.opacity = progress;
                nav.style.transform = `translateY(${-20 + 20 * progress}px)`;
                
                if (progress < 1) {
                    requestAnimationFrame(showNav);
                } else {
                    nav.style.transform = 'translateY(0)';
                }
            }
            
            requestAnimationFrame(showNav);
        }
    }
});

// 键盘快捷键支持
document.addEventListener('keydown', function(e) {
    if (e.ctrlKey || e.metaKey) {
        switch (e.key) {
            case 'k':
                e.preventDefault();
                console.log('快捷键 Ctrl+K 被触发');
                break;
            case 'm':
                e.preventDefault();
                document.querySelector('.menu-toggle')?.click();
                break;
            case 'l':
                e.preventDefault();
                const newTheme = appManager.currentTheme === 'light' ? 'dark' : 'light';
                appManager.switchTheme(newTheme);
                break;
        }
    }
});

// 页面可见性变化监听
document.addEventListener('visibilitychange', function() {
    if (document.hidden) {
        console.log('页面隐藏');
    } else {
        console.log('页面可见');
    }
});

// 页面加载完成后的初始化
document.addEventListener('DOMContentLoaded', function() {
    // 恢复保存的主题
    const savedTheme = localStorage.getItem('chtl-theme') || 'light';
    appManager.switchTheme(savedTheme);
    
    // 初始化应用
    appManager.initializeApp();
    
    // 页面加载动画
    const startTime = performance.now();
    const duration = 800;
    
    function fadeInPage(currentTime) {
        const elapsed = currentTime - startTime;
        const progress = Math.min(elapsed / duration, 1);
        
        document.body.style.opacity = progress;
        
        if (progress < 1) {
            requestAnimationFrame(fadeInPage);
        }
    }
    
    document.body.style.opacity = '0';
    requestAnimationFrame(fadeInPage);
    
    console.log('CHTL开发者项目加载完成');
});

// 页面卸载前保存状态
window.addEventListener('beforeunload', function() {
    localStorage.setItem('chtl-last-visit', new Date().toISOString());
});

// 添加CSS动画样式
const animationStyles = document.createElement('style');
animationStyles.textContent = `
    @keyframes slideIn {
        from { transform: translateX(100%); opacity: 0; }
        to { transform: translateX(0); opacity: 1; }
    }
    
    @keyframes pulse {
        0%, 100% { transform: scale(1); }
        50% { transform: scale(1.05); }
    }
    
    @keyframes bounce {
        0%, 100% { transform: translateY(0); }
        50% { transform: translateY(-10px); }
    }
    
    .pulse-animation {
        animation: pulse 2s infinite;
    }
    
    .bounce-animation {
        animation: bounce 1s infinite;
    }
`;
document.head.appendChild(animationStyles);

console.log('CHTL开发者项目JavaScript初始化完成');
