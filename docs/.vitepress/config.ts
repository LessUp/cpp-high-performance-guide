import { defineConfig } from 'vitepress'

// Site configuration constants - single source of truth
const BASE_PATH = '/cpp-high-performance-guide/'
const DEFAULT_LOCALE = 'en'
const LOCALE_MAP: Record<string, string> = {
  zh: 'zh', // Chinese -> /zh/
  // All other languages default to /en/
}

// Generate locale redirect script with dynamic base path
function generateLocaleRedirectScript(): string {
  return `(()=>{const b='${BASE_PATH}',k='lr',s=sessionStorage,p=location.pathname;if(s.getItem(k))return;if(p!==b&&p!==b.slice(0,-1))return;s.setItem(k,'1');const l=(navigator.language||'${DEFAULT_LOCALE}').toLowerCase().slice(0,2),t=LOCALE_MAP[l]||'${DEFAULT_LOCALE}';location.replace(b+t+'/'+location.search)})()`
    .replace('LOCALE_MAP', JSON.stringify(LOCALE_MAP))
}

export default defineConfig({
  title: 'C++ High Performance Guide',
  description:
    'Runnable C++20 performance engineering examples covering CMake, memory layout, SIMD, concurrency, and profiling.',
  base: BASE_PATH,
  cleanUrls: true,
  lastUpdated: true,
  head: [
    ['link', { rel: 'icon', type: 'image/svg+xml', href: `${BASE_PATH}favicon.svg` }],
    ['meta', { name: 'theme-color', content: '#5c7cfa' }],
    ['meta', { property: 'og:type', content: 'website' }],
    ['meta', { property: 'og:site_name', content: 'C++ High Performance Guide' }],
    ['meta', { property: 'og:title', content: 'C++ High Performance Guide' }],
    ['meta', { property: 'og:description', content: 'Runnable C++20 performance engineering examples and learning docs.' }],
    ['meta', { property: 'og:image', content: `https://lessup.github.io${BASE_PATH}logo.svg` }],
    ['meta', { name: 'twitter:card', content: 'summary_large_image' }],
    ['meta', { name: 'keywords', content: 'C++, C++20, performance, SIMD, cache optimization, concurrency, CMake, benchmark' }],
    // Auto-redirect based on browser language (runs before page renders)
    ['script', {}, generateLocaleRedirectScript()],
  ],
  themeConfig: {
    logo: '/logo.svg',
    siteTitle: 'C++ HPC Guide',
    search: { provider: 'local' },
    socialLinks: [
      { icon: 'github', link: 'https://github.com/LessUp/cpp-high-performance-guide', ariaLabel: 'GitHub' },
    ],
    editLink: {
      pattern: 'https://github.com/LessUp/cpp-high-performance-guide/edit/master/docs/:path',
      text: 'Edit this page on GitHub',
    },
    footer: {
      message: 'Released under the MIT License.',
      copyright: `Copyright © ${new Date().getFullYear()} C++ HPC Guide Contributors`,
    },
    outline: {
      level: [2, 3],
    },
  },
  locales: {
    root: {
      label: 'English',
      lang: 'en',
      link: '/en/',
      themeConfig: {
        nav: [
          {
            text: 'Start',
            items: [
              { text: 'Docs landing', link: '/en/' },
              { text: 'Quick Start', link: '/en/getting-started/quickstart' },
              { text: 'Installation', link: '/en/getting-started/installation' },
            ],
          },
          {
            text: 'Learn',
            items: [
              { text: 'Learning Path', link: '/en/guides/learning-path' },
              { text: 'Optimization Decision Tree', link: '/en/guides/optimization-decision-tree' },
              { text: 'Profiling Guide', link: '/en/guides/profiling-guide' },
              { text: 'Validation & Sanitizers', link: '/en/guides/validation' },
              { text: 'Best Practices', link: '/en/guides/best-practices' },
            ],
          },
          {
            text: 'Reference',
            items: [
              { text: 'FAQ', link: '/en/reference/faq' },
              { text: 'Troubleshooting', link: '/en/reference/troubleshooting' },
              { text: 'API Reference', link: '/en/reference/api-reference' },
            ],
          },
          {
            text: 'Contributing',
            items: [
              { text: 'AI Workflow', link: '/en/contributing/ai-workflow' },
              { text: 'Repository', link: 'https://github.com/LessUp/cpp-high-performance-guide' },
            ],
          },
        ],
        sidebar: {
          '/en/': [
            {
              text: 'Getting Started',
              items: [
                { text: 'Docs landing', link: '/en/' },
                { text: 'Quick Start', link: '/en/getting-started/quickstart' },
                { text: 'Installation', link: '/en/getting-started/installation' },
                { text: 'Prerequisites', link: '/en/getting-started/prerequisites' },
              ],
            },
            {
              text: 'Guides',
              items: [
                { text: 'Learning Path', link: '/en/guides/learning-path' },
                { text: 'Optimization Decision Tree', link: '/en/guides/optimization-decision-tree' },
                { text: 'Profiling Guide', link: '/en/guides/profiling-guide' },
                { text: 'Validation & Sanitizers', link: '/en/guides/validation' },
                { text: 'Best Practices', link: '/en/guides/best-practices' },
              ],
            },
            {
              text: 'Exercises',
              items: [
                { text: 'Overview', link: '/en/exercises/' },
                { text: 'Memory', link: '/en/exercises/module-02-memory' },
                { text: 'SIMD', link: '/en/exercises/module-04-simd' },
                { text: 'Concurrency', link: '/en/exercises/module-05-concurrency' },
                { text: 'Solutions', link: '/en/exercises/solutions' },
              ],
            },
            {
              text: 'Reference',
              items: [
                { text: 'FAQ', link: '/en/reference/faq' },
                { text: 'Troubleshooting', link: '/en/reference/troubleshooting' },
                { text: 'API Reference', link: '/en/reference/api-reference' },
                { text: 'Memory Utilities', link: '/en/reference/api/memory-utils' },
                { text: 'SIMD Wrapper', link: '/en/reference/api/simd-wrapper' },
                { text: 'Benchmark Utils', link: '/en/reference/api/benchmark-utils' },
              ],
            },
            {
              text: 'Contributing',
              items: [
                { text: 'AI Workflow', link: '/en/contributing/ai-workflow' },
              ],
            },
          ],
        },
      },
    },
    zh: {
      label: '简体中文',
      lang: 'zh-CN',
      link: '/zh/',
      description: '可运行的 C++20 性能工程示例与学习文档。',
      head: [
        ['meta', { property: 'og:locale', content: 'zh_CN' }],
      ],
      themeConfig: {
        editLink: {
          pattern: 'https://github.com/LessUp/cpp-high-performance-guide/edit/master/docs/:path',
          text: '在 GitHub 上编辑此页',
        },
        footer: {
          message: '基于 MIT 许可证发布。',
          copyright: `版权所有 © ${new Date().getFullYear()} C++ HPC Guide 贡献者`,
        },
        nav: [
          {
            text: '开始',
            items: [
              { text: '文档入口', link: '/zh/' },
              { text: '快速开始', link: '/zh/getting-started/quickstart' },
              { text: '安装指南', link: '/zh/getting-started/installation' },
            ],
          },
          {
            text: '学习',
            items: [
              { text: '学习路径', link: '/zh/guides/learning-path' },
              { text: '优化决策树', link: '/zh/guides/optimization-decision-tree' },
              { text: '性能分析指南', link: '/zh/guides/profiling-guide' },
              { text: '验证与 Sanitizer', link: '/zh/guides/validation' },
              { text: '最佳实践', link: '/zh/guides/best-practices' },
            ],
          },
          {
            text: '参考',
            items: [
              { text: '常见问题', link: '/zh/reference/faq' },
              { text: '故障排查', link: '/zh/reference/troubleshooting' },
              { text: 'API 入口', link: '/zh/reference/api-reference' },
            ],
          },
          {
            text: '贡献',
            items: [
              { text: 'AI 开发流程', link: '/zh/contributing/ai-workflow' },
              { text: '仓库主页', link: 'https://github.com/LessUp/cpp-high-performance-guide' },
            ],
          },
        ],
        sidebar: {
          '/zh/': [
            {
              text: '入门',
              items: [
                { text: '文档入口', link: '/zh/' },
                { text: '快速开始', link: '/zh/getting-started/quickstart' },
                { text: '安装指南', link: '/zh/getting-started/installation' },
                { text: '先决条件', link: '/zh/getting-started/prerequisites' },
              ],
            },
            {
              text: '指南',
              items: [
                { text: '学习路径', link: '/zh/guides/learning-path' },
                { text: '优化决策树', link: '/zh/guides/optimization-decision-tree' },
                { text: '性能分析指南', link: '/zh/guides/profiling-guide' },
                { text: '验证与 Sanitizer', link: '/zh/guides/validation' },
                { text: '最佳实践', link: '/zh/guides/best-practices' },
              ],
            },
            {
              text: '练习',
              items: [
                { text: '练习（英文）', link: '/en/exercises/' },
              ],
            },
            {
              text: '参考',
              items: [
                { text: '常见问题', link: '/zh/reference/faq' },
                { text: '故障排查', link: '/zh/reference/troubleshooting' },
                { text: 'API 入口', link: '/zh/reference/api-reference' },
                { text: 'API 参考（英文）', link: '/en/reference/api/memory-utils' },
              ],
            },
            {
              text: '贡献',
              items: [
                { text: 'AI 开发流程', link: '/zh/contributing/ai-workflow' },
              ],
            },
          ],
        },
        docFooter: {
          prev: '上一页',
          next: '下一页',
        },
        outline: {
          label: '本页目录',
        },
        lastUpdated: {
          text: '最后更新',
        },
        langMenuLabel: '切换语言',
        returnToTopLabel: '返回顶部',
        sidebarMenuLabel: '菜单',
        darkModeSwitchLabel: '外观',
      },
    },
  },
  markdown: {
    theme: {
      light: 'github-light',
      dark: 'github-dark',
    },
  },
})
