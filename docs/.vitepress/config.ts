import { defineConfig } from 'vitepress'

const rawBase = process.env.VITEPRESS_BASE
const base = rawBase
  ? rawBase.startsWith('/')
    ? rawBase.endsWith('/') ? rawBase : `${rawBase}/`
    : `/${rawBase}/`
  : '/'

const siteTitle = 'C++ 高性能指南'
const siteDescription = '可验证的现代 C++20 性能工程：内存布局、SIMD、无锁并发、构建系统，全部附带可运行代码与基准测试。'
const currentYear = new Date().getFullYear()

export default defineConfig({
  title: siteTitle,
  description: siteDescription,
  base,
  cleanUrls: false,
  lastUpdated: true,
  head: [
    ['link', { rel: 'icon', type: 'image/svg+xml', href: `${base}logo.svg` }],
    ['meta', { name: 'theme-color', content: '#3476f6' }],
    ['meta', { property: 'og:type', content: 'website' }],
    ['meta', { property: 'og:site_name', content: siteTitle }],
    ['meta', { property: 'og:title', content: siteTitle }],
    ['meta', { property: 'og:description', content: siteDescription }],
    ['meta', { property: 'og:locale', content: 'zh_CN' }],
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
          { text: '入门', link: '/zh/getting-started', activeMatch: '/zh/getting-started' },
          { text: '深度专题', link: '/zh/deep-dives/memory-layout', activeMatch: '/zh/deep-dives/' },
          { text: '实战指南', link: '/zh/guides/profiling', activeMatch: '/zh/guides/' },
          { text: '参考', link: '/zh/reference/api', activeMatch: '/zh/reference/' },
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
              items: [
                { text: '快速开始', link: '/zh/getting-started' },
              ],
            },
            {
              text: '深度专题',
              items: [
                { text: '内存布局与缓存', link: '/zh/deep-dives/memory-layout' },
                { text: 'SIMD 向量化', link: '/zh/deep-dives/simd-internals' },
                { text: 'MPI 分布式并行', link: '/zh/deep-dives/mpi-distributed' },
                { text: '无锁并发', link: '/zh/deep-dives/lock-free-queue' },
                { text: '线程亲和与 NUMA', link: '/zh/deep-dives/thread-affinity-numa' },
                { text: 'I/O 性能', link: '/zh/deep-dives/io-performance' },
                { text: 'GPU 卸载加速', link: '/zh/deep-dives/gpu-offloading' },
                { text: '现代 CMake 构建', link: '/zh/deep-dives/cmake-build-system' },
                { text: 'C++20 性能实践', link: '/zh/deep-dives/modern-cpp-perf' },
              ],
            },
            {
              text: '实战指南',
              items: [
                { text: '性能分析实战', link: '/zh/guides/profiling' },
                { text: '优化决策手册', link: '/zh/guides/optimization-playbook' },
              ],
            },
            {
              text: '参考',
              items: [
                { text: 'API 参考', link: '/zh/reference/api' },
                { text: '故障排查', link: '/zh/reference/troubleshooting' },
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
  themeConfig: {
    outline: [2, 3],
    search: { provider: 'local' },
    logo: {
      light: '/logo.svg',
      dark: '/logo.svg',
    },
    socialLinks: [
      { icon: 'github', link: 'https://github.com/LessUp/cpp-high-performance-guide' },
    ],
  },
})
