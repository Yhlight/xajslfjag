
// 张小明的CHTL博客JavaScript - 由CHTL编译器生成

(function() {
    'use strict';
    
    console.log('🎉 张小明的CHTL博客加载完成！');
    console.log('✨ CHTL编译器真的太棒了！');
    
    // 页面加载动画
    document.addEventListener('DOMContentLoaded', function() {
        // CHTL JS动画实现
        const startTime = performance.now();
        const duration = 800;
        
        function fadeInPage(currentTime) {
            const elapsed = currentTime - startTime;
            const progress = Math.min(elapsed / duration, 1);
            
            // ease-out缓动
            const easeOut = 1 - Math.pow(1 - progress, 3);
            
            document.body.style.opacity = easeOut;
            
            if (progress < 1) {
                requestAnimationFrame(fadeInPage);
            }
        }
        
        requestAnimationFrame(fadeInPage);
    });
    
    // 虚拟对象：页面控制器
    var pageController = {
        showWelcomeMessage: function() {
            alert('欢迎来到张小明的CHTL博客！这是用CHTL编译器创建的！');
        },
        
        highlightFeatures: function() {
            const cards = document.querySelectorAll('.blog-grid .card');
            cards.forEach(function(card, index) {
                setTimeout(function() {
                    // CHTL JS动画
                    const startTime = performance.now();
                    const duration = 500;
                    
                    function highlightAnimation(currentTime) {
                        const elapsed = currentTime - startTime;
                        const progress = Math.min(elapsed / duration, 1);
                        
                        const scale = 1 + 0.05 * Math.sin(progress * Math.PI);
                        card.style.transform = `scale(${scale})`;
                        
                        if (progress < 1) {
                            requestAnimationFrame(highlightAnimation);
                        } else {
                            card.style.transform = 'scale(1)';
                        }
                    }
                    
                    requestAnimationFrame(highlightAnimation);
                }, index * 200);
            });
        }
    };
    
    // 按钮点击动画 - CHTL JS事件绑定实现
    document.querySelectorAll('button').forEach(function(button) {
        button.addEventListener('click', function(e) {
            // CHTL JS动画效果
            const startTime = performance.now();
            const duration = 300;
            
            function buttonClickAnimation(currentTime) {
                const elapsed = currentTime - startTime;
                const progress = Math.min(elapsed / duration, 1);
                
                if (progress < 0.5) {
                    const scale = 1 + progress * 0.2;
                    this.style.transform = `scale(${scale})`;
                } else {
                    const scale = 1.1 - (progress - 0.5) * 0.2;
                    this.style.transform = `scale(${scale})`;
                }
                
                if (progress < 1) {
                    requestAnimationFrame(buttonClickAnimation.bind(this));
                } else {
                    this.style.transform = 'scale(1)';
                }
            }
            
            requestAnimationFrame(buttonClickAnimation.bind(this));
            
            console.log('哇！CHTL JS动画太流畅了！');
        });
    });
    
    // Chtholly手风琴交互
    const accordionHeaders = document.querySelectorAll('.accordion-header');
    accordionHeaders.forEach(function(header) {
        header.addEventListener('click', function() {
            const accordion = this.closest('.chtholly-accordion');
            accordion.classList.toggle('active');
            
            console.log('手风琴切换！珂朵莉的魔法生效了！');
        });
    });
    
    // Yuigahama音乐播放器交互
    const playButton = document.querySelector('.player-btn.play');
    const playerCover = document.querySelector('.player-cover');
    
    if (playButton && playerCover) {
        playButton.addEventListener('click', function() {
            const isPlaying = playerCover.classList.contains('playing');
            
            if (isPlaying) {
                playerCover.classList.remove('playing');
                this.textContent = '▶';
                console.log('⏸️ 音乐暂停');
            } else {
                playerCover.classList.add('playing');
                this.textContent = '⏸';
                console.log('🎵 开始播放由比滨结衣的音乐！');
            }
        });
    }
    
    // 3秒后显示欢迎消息
    setTimeout(function() {
        pageController.showWelcomeMessage();
    }, 3000);
    
    // 5秒后高亮功能卡片
    setTimeout(function() {
        pageController.highlightFeatures();
    }, 5000);
    
})();
