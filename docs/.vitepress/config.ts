import { defineConfig } from 'vitepress'
import { withMermaid } from 'vitepress-plugin-mermaid'
import llmstxt from 'vitepress-plugin-llms'
import { englishReferenceHub } from './config-helpers.js'

const rawBase = process.env.VITEPRESS_BASE
const base = rawBase
  ? rawBase.startsWith('/')
    ? rawBase.endsWith('/') ? rawBase : `${rawBase}/`
    : `/${rawBase}/`
  : '/'

const siteTitle = 'C++ High Performance Guide'
const siteDescription =
  'Runnable C++ performance engineering docs covering academy, architecture, playbooks, references, and research.'
const zhSiteTitle = 'C++ 高性能指南'
const zhSiteDescription = '可运行的 C++ 性能工程文档，覆盖学院、架构、实践手册、参考资料与研究内容。'
const siteUrl = 'https://lessup.github.io'
const canonicalBase = '/cpp-high-performance-guide/'
const currentYear = new Date().getFullYear()

function assetPath(path: string): string {
  const normalizedPath = path.replace(/^\/+/, '')
  return `${base}${normalizedPath}`
}

function canonicalAssetUrl(path: string): string {
  const normalizedPath = path.replace(/^\/+/, '')
  return new URL(`${canonicalBase.replace(/^\/+/, '')}${normalizedPath}`, `${siteUrl}/`).toString()
}

const enAcademyItems = [
  { text: 'Academy Overview', link: '/en/academy/' },
  { text: 'Module Atlas', link: '/en/academy/module-atlas' },
  { text: 'Validation Doctrine', link: '/en/academy/validation-doctrine' },
]

const enArchitectureItems = [
  { text: 'Architecture Overview', link: '/en/architecture/' },
  { text: 'Repository Topology', link: '/en/architecture/repository-topology' },
  { text: 'Performance Methodology', link: '/en/architecture/performance-methodology' },
]

const enPlaybookItems = [
  { text: 'Playbook Overview', link: '/en/playbook/' },
  { text: 'Quick Start', link: '/en/getting-started/quickstart' },
  { text: 'Installation', link: '/en/getting-started/installation' },
  { text: 'Prerequisites', link: '/en/getting-started/prerequisites' },
  { text: 'Learning Path', link: '/en/guides/learning-path' },
  { text: 'Profiling Guide', link: '/en/guides/profiling-guide' },
  { text: 'Validation & Sanitizers', link: '/en/guides/validation' },
  { text: 'Optimization Decision Tree', link: '/en/guides/optimization-decision-tree' },
  { text: 'Best Practices', link: '/en/guides/best-practices' },
]

const enReferenceItems = [
  { text: 'Reference Overview', link: '/en/reference/' },
  { text: 'API Reference', link: '/en/reference/api-reference' },
  { text: 'FAQ', link: '/en/reference/faq' },
  { text: 'Troubleshooting', link: '/en/reference/troubleshooting' },
  { text: 'Memory Utilities', link: '/en/reference/api/memory-utils' },
  { text: 'SIMD Wrapper', link: '/en/reference/api/simd-wrapper' },
  { text: 'Benchmark Utils', link: '/en/reference/api/benchmark-utils' },
]

const enResearchItems = [
  { text: 'Research Overview', link: '/en/research/' },
  { text: 'Related Work', link: '/en/research/related-work' },
  { text: 'References', link: '/en/research/references' },
  { text: 'Evolution', link: '/en/research/evolution' },
]

const enContributingItems = [
  { text: 'AI Workflow', link: '/en/contributing/ai-workflow' },
]

const enExerciseItems = [
  { text: 'Overview', link: '/en/exercises/' },
  { text: 'Memory', link: '/en/exercises/module-02-memory' },
  { text: 'SIMD', link: '/en/exercises/module-04-simd' },
  { text: 'Concurrency', link: '/en/exercises/module-05-concurrency' },
  { text: 'Solutions', link: '/en/exercises/solutions' },
]

const zhAcademyItems = [
  { text: '学院概览', link: '/zh/academy/' },
  { text: '模块总览', link: '/zh/academy/module-atlas' },
  { text: '验证原则', link: '/zh/academy/validation-doctrine' },
]

const zhArchitectureItems = [
  { text: '架构概览', link: '/zh/architecture/' },
  { text: '仓库拓扑', link: '/zh/architecture/repository-topology' },
  { text: '性能方法论', link: '/zh/architecture/performance-methodology' },
]

const zhPlaybookItems = [
  { text: '实践手册概览', link: '/zh/playbook/' },
  { text: '快速开始', link: '/zh/getting-started/quickstart' },
  { text: '安装指南', link: '/zh/getting-started/installation' },
  { text: '先决条件', link: '/zh/getting-started/prerequisites' },
  { text: '学习路径', link: '/zh/guides/learning-path' },
  { text: '性能分析指南', link: '/zh/guides/profiling-guide' },
  { text: '验证与 Sanitizer', link: '/zh/guides/validation' },
  { text: '优化决策树', link: '/zh/guides/optimization-decision-tree' },
  { text: '最佳实践', link: '/zh/guides/best-practices' },
]

const zhReferenceItems = [
  { text: '参考概览', link: '/zh/reference/' },
  { text: 'API 入口', link: '/zh/reference/api-reference' },
  { text: '常见问题', link: '/zh/reference/faq' },
  { text: '故障排查', link: '/zh/reference/troubleshooting' },
  { text: '参考概览（英文）', link: englishReferenceHub },
]

const zhResearchItems = [
  { text: '研究概览', link: '/zh/research/' },
  { text: '相关工作', link: '/zh/research/related-work' },
  { text: '参考资料', link: '/zh/research/references' },
  { text: '演进记录', link: '/zh/research/evolution' },
]

const zhContributingItems = [
  { text: 'AI 开发流程', link: '/zh/contributing/ai-workflow' },
]

const zhExerciseItems = [
  { text: '练习总览（英文）', link: '/en/exercises/' },
  { text: '内存练习（英文）', link: '/en/exercises/module-02-memory' },
  { text: 'SIMD 练习（英文）', link: '/en/exercises/module-04-simd' },
  { text: '并发练习（英文）', link: '/en/exercises/module-05-concurrency' },
  { text: '解答（英文）', link: '/en/exercises/solutions' },
]

export default withMermaid(defineConfig({
  title: siteTitle,
  description: siteDescription,
  base,
  cleanUrls: false,
  lastUpdated: true,
  head: [
    ['link', { rel: 'icon', type: 'image/svg+xml', href: assetPath('/logo.svg') }],
    ['meta', { name: 'theme-color', content: '#3476f6' }],
    ['meta', { property: 'og:type', content: 'website' }],
    ['meta', { property: 'og:site_name', content: siteTitle }],
    ['meta', { property: 'og:title', content: siteTitle }],
    ['meta', { property: 'og:description', content: siteDescription }],
    ['meta', { property: 'og:image', content: canonicalAssetUrl('/og-card.png') }],
    ['meta', { property: 'og:image:alt', content: 'C++ High Performance Guide documentation social card' }],
    ['meta', { name: 'twitter:card', content: 'summary_large_image' }],
    ['meta', { name: 'twitter:image', content: canonicalAssetUrl('/og-card.png') }],
    ['meta', { name: 'twitter:title', content: siteTitle }],
    ['meta', { name: 'twitter:description', content: siteDescription }],
    ['meta', { name: 'keywords', content: 'C++, performance, academy, architecture, playbook, reference, research, SIMD, concurrency, CMake, benchmark' }],
  ],
  locales: {
    en: {
      label: 'English',
      lang: 'en-US',
      link: '/en/',
      title: siteTitle,
      description: siteDescription,
      head: [
        ['meta', { property: 'og:locale', content: 'en_US' }],
      ],
      themeConfig: {
        nav: [
          { text: 'Academy', link: '/en/academy/', activeMatch: '/en/academy/' },
          { text: 'Architecture', link: '/en/architecture/', activeMatch: '/en/architecture/' },
          { text: 'Playbook', link: '/en/playbook/', activeMatch: '/en/playbook/|/en/getting-started/|/en/guides/' },
          { text: 'Reference', link: '/en/reference/', activeMatch: '/en/reference/' },
          { text: 'Research', link: '/en/research/', activeMatch: '/en/research/' },
          { text: 'Contributing', link: '/en/contributing/ai-workflow', activeMatch: '/en/contributing/' },
        ],
        sidebar: {
          '/en/': [
            {
              text: 'Overview',
              items: [
                { text: 'Docs landing', link: '/en/' },
              ],
            },
            {
              text: 'Academy',
              items: enAcademyItems,
            },
            {
              text: 'Architecture',
              items: enArchitectureItems,
            },
            {
              text: 'Playbook',
              items: enPlaybookItems,
            },
            {
              text: 'Reference',
              items: enReferenceItems,
            },
            {
              text: 'Research',
              items: enResearchItems,
            },
            {
              text: 'Contributing',
              items: enContributingItems,
            },
            {
              text: 'Exercises (secondary)',
              items: enExerciseItems,
            },
          ],
        },
      },
    },
    zh: {
      label: '简体中文',
      lang: 'zh-CN',
      link: '/zh/',
      title: zhSiteTitle,
      description: zhSiteDescription,
      head: [
        ['meta', { property: 'og:locale', content: 'zh_CN' }],
        ['meta', { property: 'og:title', content: zhSiteTitle }],
        ['meta', { property: 'og:description', content: zhSiteDescription }],
        ['meta', { property: 'og:image:alt', content: 'C++ 高性能指南文档社交卡片' }],
        ['meta', { name: 'twitter:title', content: zhSiteTitle }],
        ['meta', { name: 'twitter:description', content: zhSiteDescription }],
      ],
      themeConfig: {
        editLink: {
          pattern: 'https://github.com/LessUp/cpp-high-performance-guide/edit/master/docs/:path',
          text: '在 GitHub 上编辑此页',
        },
        footer: {
          message: '基于 MIT 许可证发布。',
          copyright: `版权所有 © ${currentYear} C++ HPC Guide 贡献者`,
        },
        nav: [
          { text: '学院', link: '/zh/academy/', activeMatch: '/zh/academy/' },
          { text: '架构', link: '/zh/architecture/', activeMatch: '/zh/architecture/' },
          { text: '实践手册', link: '/zh/playbook/', activeMatch: '/zh/playbook/|/zh/getting-started/|/zh/guides/' },
          { text: '参考', link: '/zh/reference/', activeMatch: '/zh/reference/' },
          { text: '研究', link: '/zh/research/', activeMatch: '/zh/research/' },
          { text: '贡献', link: '/zh/contributing/ai-workflow', activeMatch: '/zh/contributing/' },
        ],
        sidebar: {
          '/zh/': [
            {
              text: '概览',
              items: [
                { text: '文档入口', link: '/zh/' },
              ],
            },
            {
              text: '学院',
              items: zhAcademyItems,
            },
            {
              text: '架构',
              items: zhArchitectureItems,
            },
            {
              text: '实践手册',
              items: zhPlaybookItems,
            },
            {
              text: '参考',
              items: zhReferenceItems,
            },
            {
              text: '研究',
              items: zhResearchItems,
            },
            {
              text: '贡献',
              items: zhContributingItems,
            },
            {
              text: '练习（次要，英文优先）',
              items: zhExerciseItems,
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
  themeConfig: {
    outline: [2, 3],
    search: { provider: 'local' },
    logo: {
      light: '/logo.svg',
      dark: '/logo.svg',
    },
    editLink: {
      pattern: 'https://github.com/LessUp/cpp-high-performance-guide/edit/master/docs/:path',
      text: 'Edit this page on GitHub',
    },
    footer: {
      message: 'Released under the MIT License.',
      copyright: `Copyright © ${currentYear} C++ HPC Guide contributors`,
    },
    socialLinks: [
      { icon: 'github', link: 'https://github.com/LessUp/cpp-high-performance-guide' },
    ],
  },
  vite: {
    plugins: [llmstxt()],
  },
}))
