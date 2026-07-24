import { defineConfig } from 'vitepress'
import { withMermaid } from 'vitepress-plugin-mermaid'
import llmstxt from 'vitepress-plugin-llms'

const rawBase = process.env.VITEPRESS_BASE
const base = rawBase
  ? rawBase.startsWith('/')
    ? rawBase.endsWith('/') ? rawBase : `${rawBase}/`
    : `/${rawBase}/`
  : '/'

const siteTitle = 'C++ 高性能指南'
const siteDescription = '可运行的 C++ 性能工程文档，覆盖入门、指南、深度专题、算法、练习与参考。'
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

const zhGettingStartedItems = [
  { text: '先决条件', link: '/zh/getting-started/prerequisites' },
  { text: '安装指南', link: '/zh/getting-started/installation' },
  { text: '快速开始', link: '/zh/getting-started/quickstart' },
]

const zhGuidesItems = [
  { text: '学习路径', link: '/zh/guides/learning-path' },
  { text: '性能分析指南', link: '/zh/guides/profiling-guide' },
  { text: '验证与 Sanitizer', link: '/zh/guides/validation' },
  { text: '优化决策树', link: '/zh/guides/optimization-decision-tree' },
  { text: '最佳实践', link: '/zh/guides/best-practices' },
]

const zhDeepDivesItems = [
  { text: '内存布局', link: '/zh/deep-dives/memory-layout' },
  { text: '无锁队列', link: '/zh/deep-dives/lock-free-queue' },
  { text: 'SIMD 内部机制', link: '/zh/deep-dives/simd-internals' },
]

const zhAlgorithmsItems = [
  { text: '排序算法', link: '/zh/algorithms/sorting' },
  { text: '哈希算法', link: '/zh/algorithms/hashing' },
]

const zhExerciseItems = [
  { text: '内存练习', link: '/zh/exercises/module-02-memory' },
  { text: 'SIMD 练习', link: '/zh/exercises/module-04-simd' },
  { text: '并发练习', link: '/zh/exercises/module-05-concurrency' },
  { text: '解答', link: '/zh/exercises/solutions' },
]

const zhReferenceItems = [
  { text: '内存工具', link: '/zh/reference/api/memory-utils' },
  { text: 'SIMD Wrapper', link: '/zh/reference/api/simd-wrapper' },
  { text: 'Benchmark 工具', link: '/zh/reference/api/benchmark-utils' },
  { text: '常见问题', link: '/zh/reference/faq' },
  { text: '故障排查', link: '/zh/reference/troubleshooting' },
]

export default withMermaid(defineConfig({
  title: siteTitle,
  description: siteDescription,
  base,
  cleanUrls: false,
  lastUpdated: true,
  mermaid: {
    startOnLoad: false,
    securityLevel: 'strict',
    flowchart: { useMaxWidth: true, htmlLabels: true },
    sequence: { useMaxWidth: true },
    gantt: { useMaxWidth: true },
    theme: 'base',
    themeVariables: {
      fontFamily: 'Inter, system-ui, sans-serif',
      fontSize: '14px',
    },
  },
  head: [
    ['link', { rel: 'icon', type: 'image/svg+xml', href: assetPath('/logo.svg') }],
    ['meta', { name: 'theme-color', content: '#3476f6' }],
    ['meta', { property: 'og:type', content: 'website' }],
    ['meta', { property: 'og:site_name', content: siteTitle }],
    ['meta', { property: 'og:title', content: siteTitle }],
    ['meta', { property: 'og:description', content: siteDescription }],
    ['meta', { property: 'og:image', content: canonicalAssetUrl('/og-card.png') }],
    ['meta', { property: 'og:image:alt', content: 'C++ 高性能指南文档社交卡片' }],
    ['meta', { property: 'og:locale', content: 'zh_CN' }],
    ['meta', { name: 'twitter:card', content: 'summary_large_image' }],
    ['meta', { name: 'twitter:image', content: canonicalAssetUrl('/og-card.png') }],
    ['meta', { name: 'twitter:title', content: siteTitle }],
    ['meta', { name: 'twitter:description', content: siteDescription }],
    ['meta', { name: 'keywords', content: 'C++, 性能, SIMD, 并发, CMake, 基准测试, 内存布局, 无锁队列' }],
  ],
  locales: {
    zh: {
      label: '简体中文',
      lang: 'zh-CN',
      link: '/zh/',
      title: siteTitle,
      description: siteDescription,
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
          { text: '入门', link: '/zh/getting-started/quickstart', activeMatch: '/zh/getting-started/' },
          { text: '指南', link: '/zh/guides/learning-path', activeMatch: '/zh/guides/' },
          { text: '深度专题', link: '/zh/deep-dives/memory-layout', activeMatch: '/zh/deep-dives/' },
          { text: '算法', link: '/zh/algorithms/sorting', activeMatch: '/zh/algorithms/' },
          { text: '练习', link: '/zh/exercises/module-02-memory', activeMatch: '/zh/exercises/' },
          { text: '参考', link: '/zh/reference/faq', activeMatch: '/zh/reference/' },
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
              text: '入门',
              items: zhGettingStartedItems,
            },
            {
              text: '指南',
              items: zhGuidesItems,
            },
            {
              text: '深度专题',
              items: zhDeepDivesItems,
            },
            {
              text: '算法',
              items: zhAlgorithmsItems,
            },
            {
              text: '练习',
              items: zhExerciseItems,
            },
            {
              text: '参考',
              items: zhReferenceItems,
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
      dark: '/logo-dark.svg',
    },
    socialLinks: [
      { icon: 'github', link: 'https://github.com/LessUp/cpp-high-performance-guide' },
    ],
  },
  vite: {
    plugins: [llmstxt()],
    build: {
      rollupOptions: {
        output: {
          manualChunks(id) {
            if (id.includes('mermaid')) return 'mermaid-lazy'
            if (id.includes('katex')) return 'katex-lazy'
          },
        },
      },
    },
  },
}))
